#include "gui.h"
#include <python3.7/Python.h>
#include <stdio.h>

/*** STATIC FUNTION ***/



/*** GLOBAL FUNCTION ***/

void Gui_RunGui(void)
{
    Py_Initialize();

    FILE* PScriptFile = fopen("gui_plot.py", "r");
    
    if(PScriptFile)
    {
        PyRun_SimpleFile(PScriptFile, "gui_plot.py");
        fclose(PScriptFile);
    } 

    Py_Finalize();
}