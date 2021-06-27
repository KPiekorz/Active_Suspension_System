#include "system_init.h"
#include "gui.h"

/* modules init functions */
typedef void (*module_init)(void);

typedef struct
{
    module_init module_init_func;
} module_init_t;

const module_init_t system_module_init[] = 
{
    {},
};

#define GetModulesCount()   ((int)(sizeof(system_module_init)/sizeof(module_init_t)))    


/*** STATIC FUNCTION ***/


/*** GLOBAL FUNCTION ***/

void SystemInit_Init(void)
{
    Gui_RunGui();

    // printf("Suspension system init...\n");

    // for (int i = 0; i < GetModulesCount(); i++)
    // {
    //     pid_t child_pid = fork();
    //     if (0 == child_pid) 
    //     {
    //         /* child code */
    //         // system_module_init[i].module_init_func();

    //         exit(EXIT_SUCCESS);
    //     }
    // }
}