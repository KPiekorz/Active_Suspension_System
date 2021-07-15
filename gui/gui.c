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
#include "model_simulation.h"

#define INCLUDE_PYTHON_GUI

const char *gui_fifo_name = "gui_fifo";

/*** STATIC FUNTION ***/

static void gui_InitUdpSocketConnectionToPythonPlot(void)
{
}

static void gui_SendDataToPlot(void *data, int data_len)
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
    // it can be pleaced in other function for creating a thread
    {
        /* Scheduling policy: FIFO or RR */
        int policy;
        /* Structure of other thread parameters */
        struct sched_param param;

        /* Read modify and set new thread priority */
        pthread_getschedparam(pthread_self(), &policy, &param);
        param.sched_priority = sched_get_priority_max(policy);
        pthread_setschedparam(pthread_self(), policy, &param);
    }

    SystemUtility_InitThread();

    // here have to be pleaced function for receiving message from other process via fifo queue

    SystemUtility_CreateMessageFifo(gui_fifo_name);

    while (true)
    {
        // SystemUtility_ReceiveMessage();

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
        DEBUG_LOG_ERROR("[GUI] gui_StartReceiveMessageTread, Can't create a thread!");
        return;
    }
}

/*** GLOBAL FUNCTION ***/

void Gui_Init(void)
{
    #ifdef  INIT_GUI
        DEBUG_LOG_INFO("[GUI] Gui_Init, Init process...");

        /* Init pipe connection to gui process */
        gui_StartReceiveMessageTread();

        /* Init udp socket connection to python gui app */
        // gui_InitUdpSocketConnectionToPythonPlot();

        /* Start gui python app */
        gui_RunGui();

        while (1)
        {
            DEBUG_LOG_VERBOSE("[GUI] Gui_Init, process running... (should never enter here).");
            DELAY_S(5);
        }

        exit(EXIT_SUCCESS);
    #else
        DEBUG_LOG_INFO("[GUI] Gui_Init, Won't be initialized.");
    #endif
}

void Gui_Destroy(void)
{
    DEBUG_LOG_DEBUG("[GUI] Destroy process...");
}

void Gui_SendMessage(gui_message_type_t message_type, float * data, int data_len)
{
    DEBUG_LOG_DEBUG("[GUI] Gui_SendMessage, message type: %d, len: %d", message_type, data_len);

    if (!SystemUtility_SendMessage(gui_fifo_name, (int)message_type, data, data_len))
    {
        DEBUG_LOG_ERROR("[GUI] Gui_SendMessage, Can't send message to gui process!");
    }
}