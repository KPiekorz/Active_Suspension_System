#include "gui.h"
#include <python3.7/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "system_utility.h"

#define INCLUDE_PYTHON_GUI

/*** STATIC FUNTION ***/

static void gui_InitUdpSocketConnectionToPythonPlot(void)
{

}

static void gui_RunGui(void)
{

#ifdef INCLUDE_PYTHON_GUI

    Py_Initialize();

    errno = 0;
    system("chmod +x ./gui/gui_plot.py");
    FILE* PScriptFile = fopen("./gui/gui_plot.py", "r");

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

/*** GLOBAL FUNCTION ***/

void Gui_Init(void)
{
    DEBUG_LOG_DEBUG("Init GUI process...");

    // /* Init udp socket connection to python gui app */
    // gui_InitUdpSocketConnectionToPythonPlot();

    /* Start gui python app */
    gui_RunGui();

    while (1)
    {
        DEBUG_LOG_VERBOSE("Gui process running...");
		usleep(SEC_TO_US(1));
    }
}

void Gui_Destroy(void)
{
    DEBUG_LOG_DEBUG("Destroy GUI process...");


}

void Gui_SendDataToPlot(void * data, uint16_t data_len)
{

}