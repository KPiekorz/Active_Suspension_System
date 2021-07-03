#include "gui.h"
#include <python3.7/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>

#include "system_utility.h"

#define INCLUDE_PYTHON_GUI

const char *gui_fifo_name = "gui_fifo";

/*** STATIC FUNTION ***/

static void gui_InitUdpSocketConnectionToPythonPlot(void)
{
}

static void gui_SendDataToPlot(void *data, uint16_t data_len)
{
}

static void gui_RunGui(void)
{

#ifdef INCLUDE_PYTHON_GUI

    Py_Initialize();

    errno = 0;
    system("chmod +x ./gui/gui_plot.py");
    FILE *PScriptFile = fopen("./gui/gui_plot.py", "r");

    if (PScriptFile == NULL)
    {
        DEBUG_LOG_ERROR("File to open py gui, Error: %d", errno);
    }
    else
    {
        PyRun_SimpleFile(PScriptFile, "gui_plot.py");
        fclose(PScriptFile);
    }

    Py_Finalize();

#endif /* INCLUDE_PYTHON_GUI */
}

static void *gui_ReceiveMessageThread(void *cookie)
{
    /* Scheduling policy: FIFO or RR */
    int policy;
    /* Structure of other thread parameters */
    struct sched_param param;

    /* Read modify and set new thread priority */
    pthread_getschedparam(pthread_self(), &policy, &param);
    param.sched_priority = sched_get_priority_max(policy);
    pthread_setschedparam(pthread_self(), policy, &param);

    int fd, bytes_read;
    char buff[DEFAULT_FIFO_SIZE];

    /* Create FIFO */
    if ((mkfifo(gui_fifo_name, 0664) == -1) && (errno != EEXIST))
    {
        DEBUG_LOG_ERROR("[GUI] Cannot create FIFO.");
        return 0;
    }

    /* Open FIFO file */
    if ((fd = open(gui_fifo_name, O_RDONLY)) == -1)
    {
        DEBUG_LOG_ERROR("[GUI] Cannot open FIFO.");
        return 0;
    }

    while (true)
    {
        DEBUG_LOG_VERBOSE("[GUI] Waiting for new message.");

        /* receive message from other modules form system */
        memset(buff, '\0', sizeof(buff));

        /* Read data from FIFO */
        if ((bytes_read = read(fd, buff, sizeof(buff))) == -1)
        {
            DEBUG_LOG_ERROR("Something is wrong with FIFO.");
            return 0;
        }

        /* If there is no data just continue */
        if (bytes_read == 0)
        {
            DELAY_MS(1000);
            continue;
        }

        /* If there is message print it */
        if (bytes_read > 2)
        {
            DEBUG_LOG_DEBUG("[GUI] gui_ReceiveMessageThread, buff: %s, type: %d, len: %d",
                            buff,
                            buff[SYSTEM_MESSAGE_TYPE_OFFSET],
                            buff[SYSTEM_MESSAGE_LENGTH_OFFSET]);
        }
        else
        {
            DEBUG_LOG_WARN("[GUI] gui_ReceiveMessageThread, Message too short!!!");
        }
    }
}

static void gui_StartReceiveMessageTread(void)
{
    /**
    *  Start a FIFO reader mean to create thread for receiving a
    * messages from other thread process among the system.
    **/

    int status;

    /* Thread variable */
    pthread_t tSimpleThread;

    /* Thread attributes variable */
    pthread_attr_t aSimpleThreadAttr;

    /* Initialize thread attributes structure for FIFO scheduling */
    pthread_attr_init(&aSimpleThreadAttr);
    pthread_attr_setschedpolicy(&aSimpleThreadAttr, SCHED_FIFO);

    /* Create thread */
    if ((status = pthread_create(&tSimpleThread, &aSimpleThreadAttr, gui_ReceiveMessageThread, NULL)))
    {
        DEBUG_LOG_ERROR("[GUI] Can't create a thread!");
        return;
    }
}

/*** GLOBAL FUNCTION ***/

void Gui_Init(void)
{
    DEBUG_LOG_DEBUG("Init GUI process...");

    /* Init pipe connection to gui process */
    gui_StartReceiveMessageTread();

    /* Init udp socket connection to python gui app */
    // gui_InitUdpSocketConnectionToPythonPlot();

    /* Start gui python app */
    // gui_RunGui();

    while (1)
    {
        DEBUG_LOG_VERBOSE("[GUI] Gui process running... (should never enter here).");
        DELAY_S(5);
    }
}

void Gui_Destroy(void)
{
    DEBUG_LOG_DEBUG("Destroy GUI process...");
}

void Gui_SendMessage(gui_message_type_t message_type, const void * data, int data_len)
{
    DEBUG_LOG_DEBUG("Gui_SendMessage, message type: %d, len: %d", message_type, data_len);

    int fd;
    char buff[GET_MESSAGE_SIZE(data_len)];
    buff[SYSTEM_MESSAGE_TYPE_OFFSET] = message_type;
    buff[SYSTEM_MESSAGE_LENGTH_OFFSET] = data_len;
    memcpy(&(buff[SYSTEM_MESSAGE_DATA_OFFSET]), (char *)data, data_len);

    /* Open FIFO file */
    if ((fd = open(gui_fifo_name, O_WRONLY)) == -1)
    {
        DEBUG_LOG_ERROR("[GUI] Cannot open FIFO.");
        return;
    }

    /* Write a message to FIFO */
    if (write(fd, buff, GET_MESSAGE_SIZE(data_len)) != strlen(buff))
    {
        DEBUG_LOG_ERROR("[GUI] Cannot write to FIFO.");
        return;
    }

    /* Close FIFO */
    if (close(fd) == -1)
    {
        DEBUG_LOG_ERROR("[GUI] Cannot close FIFO.");
        return;
    }
}