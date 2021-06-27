#include "gui.h"
#include <python3.7/Python.h>
#include <stdio.h>
#include <stdlib.h>

#define START_PYTHON_GUI

/*** STATIC FUNTION ***/



/*** GLOBAL FUNCTION ***/

void Gui_RunGui(void)
{

#ifdef START_PYTHON_GUI

    Py_Initialize();

    FILE* PScriptFile = fopen("gui_plot.py", "r");

    if(PScriptFile)
    {
        PyRun_SimpleFile(PScriptFile, "gui_plot.py");
        fclose(PScriptFile);
    } 

    Py_Finalize();

#endif /* START_PYTHON_GUI */

}