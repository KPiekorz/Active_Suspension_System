#include "gui.h"
#include <python3.7/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define START_PYTHON_GUI

/*** STATIC FUNTION ***/



/*** GLOBAL FUNCTION ***/

void Gui_RunGui(void)
{

#ifdef START_PYTHON_GUI

    Py_Initialize();

    errno = 0;
    system("chmod +x ./gui/gui_plot.py");
    FILE* PScriptFile = fopen("./gui/gui_plot.py", "r");

    if (PScriptFile == NULL) 
    {
        printf("File to open py gui, Error: %d \n\r", errno);
    }
    else
    {
        PyRun_SimpleFile(PScriptFile, "gui_plot.py");
        fclose(PScriptFile);
    }

    Py_Finalize();

#endif /* START_PYTHON_GUI */

}

void Gui_SendDataToPlot(void * data, uint16_t data_len)
{
    
}