#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "system_init.h"
#include "gui.h"
#include "control.h"
#include "model_simulation.h"
#include "sensors.h"

/* modules init functions */
typedef void (*module_init)(void);

typedef struct
{
    module_init module_init_func;
} module_init_t;

const module_init_t system_module_init[] = 
{
    {Gui_Init},
};

#define GetModulesCount()   ((int)(sizeof(system_module_init)/sizeof(module_init_t)))    

/*** STATIC FUNCTION ***/


/*** GLOBAL FUNCTION ***/

void SystemProcess_Initialize(void)
{
    printf("Suspension system init...\n");

    for (int i = 0; i < GetModulesCount(); i++)
    {
      
        // here we will create new process
        pid_t child_pid = fork();


        if (0 == child_pid) 
        {
            /* child code */
            // system_module_init[i].module_init_func();

            exit(EXIT_SUCCESS);
        }
    }
}

void SystemProcess_Destroy(void)
{
    
}