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
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self());

    /* create message fifo queue */
    if (SystemUtility_CreateMessageFifo(gui_fifo_name) == false)
    {
        DEBUG_LOG_ERROR("[GUI] gui_ReceiveMessageThread, Can't create FIFO!");
        return 0;
    }

    gui_message_type_t message_type = gui_message_unknown;
    int float_data_len = 20;
    float float_data[float_data_len];

    while (true)
    {
        DEBUG_LOG_VERBOSE("[GUI] gui_ReceiveMessageThread, Wait for message!");

        /* delay for waiting for another message */
        DELAY_MS(100);

        /* try receive message */
        if (true == SystemUtility_ReceiveMessage(gui_fifo_name, (int *)&message_type, float_data, &float_data_len))
        {
            DEBUG_LOG_VERBOSE("[GUI] gui_ReceiveMessageThread, message type: %d, float data len: %d", message_type, float_data_len);

            for (int i = 0; i < float_data_len; i++)
            {
                DEBUG_LOG_VERBOSE("[GUI] gui_ReceiveMessageThread, float[%d]: %f", i, float_data[i]);
            }
        }
    }

    return 0;
}

/*** GLOBAL FUNCTION ***/

void Gui_Init(void)
{
    #ifdef  INIT_GUI
        DEBUG_LOG_INFO("[GUI] Gui_Init, Init process...");

        /* Init pipe connection to gui process */
        if (SystemUtility_CreateThread(gui_ReceiveMessageThread))
        {
            DEBUG_LOG_ERROR("[GUI] Gui_Init, Can't create receive gui thread!");
        }

        /* Init udp socket connection to python gui app */
        // gui_InitUdpSocketConnectionToPythonPlot();

        /* Start gui python app */
        // gui_RunGui();

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