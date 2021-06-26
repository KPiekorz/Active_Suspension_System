#include "gui.h"
#include <python3.7/Python.h>
#include <stdio.h>

/*** STATIC FUNTION ***/



/*** GLOBAL FUNCTION ***/

void Gui_RunGui(void)
{
    const char filename[] = "gui_plot";
    FILE * fp;

    Py_Initialize();

	fp = _Py_fopen(filename, "r");
	PyRun_SimpleFile(fp, filename);

	Py_Finalize();    
}