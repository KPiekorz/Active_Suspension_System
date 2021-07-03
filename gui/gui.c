#include "gui.h"
#include <python3.7/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "system_utility.h"

#define INCLUDE_PYTHON_GUI

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

static void * gui_ReceiveMessageThread(void *cookie)
{
	/* Scheduling policy: FIFO or RR */
	int policy;
	/* Structure of other thread parameters */
	struct sched_param param;

	/* Read modify and set new thread priority */
	pthread_getschedparam( pthread_self(), &policy, &param);
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam( pthread_self(), policy, &param);

    while (true)
    {
        /* receive message from other modules form system */

        DEBUG_LOG_VERBOSE("[GUI] Receive message from other preocess...");
        usleep(SEC_TO_US(1));
    }
}

static void gui_StartMessageReader(void)
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
    gui_StartMessageReader();

    /* Init udp socket connection to python gui app */
    // gui_InitUdpSocketConnectionToPythonPlot();

    /* Start gui python app */
    gui_RunGui();

    while (1)
    {
        DEBUG_LOG_VERBOSE("Gui process running...");
        usleep(SEC_TO_US(5));
    }
}

void Gui_Destroy(void)
{
    DEBUG_LOG_DEBUG("Destroy GUI process...");
}

void Gui_SendMessage(gui_message_type_t message_type, void *data, uint16_t data_len)
{
    DEBUG_LOG_DEBUG("Gui_SendMessage, data len: %d", data_len);
}