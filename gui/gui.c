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
    FILE* PScriptFile = fopen("gui_plot.py", "r");

    if (PScriptFile == NULL) 
    {
        printf("Error %d \n\r", errno);
        printf("It's null\n\r");
    }
    else
    {
        printf("working\r\n");
        PyRun_SimpleFile(PScriptFile, "gui_plot.py");
        fclose(PScriptFile);
    }

    Py_Finalize();

#endif /* START_PYTHON_GUI */

}