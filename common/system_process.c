#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>

#include "system_process.h"
#include "gui.h"
#include "control.h"
#include "model_simulation.h"
#include "sensors.h"

#define UNKNOWN_PID     (0)

/* process init functions */
typedef void (* process_init)(void);

/* process dystroy function */
typedef void (* process_destory)(void);

typedef struct
{
    process_init process_init_func;
    process_destory process_destroy_func;
    pid_t process_pid;
    const char * process_name;
} process_attributes_t_t;

const process_attributes_t_t system_process_attributes[] = 
{
    {Gui_Init,          Gui_Destroy,        UNKNOWN_PID,        "Gui"},
};

#define GetProcessCount()   ((int)(sizeof(system_process_attributes)/sizeof(process_attributes_t_t)))    

/*** STATIC FUNCTION ***/

static systemProcess_PrintProcessPid(void)
{
    for (uint8_t i = 0; i < GetProcessCount(); i++)
    {
        printf ("Process %s, pid: %d\n", system_process_attributes[i].process_name, 
                                         system_process_attributes[i].process_pid);
    }
}

/*** GLOBAL FUNCTION ***/

void SystemProcess_Initialize(void)
{
    printf("Suspension system init...\n");

    for (int i = 0; i < GetProcessCount(); i++)
    {
      
        // here we will create new process
        pid_t child_pid = fork();

        if (0 == child_pid) 
        {
            /* child code */
            system_process_attributes[i].process_init_func();
        }
        else 
        {
            /* parent code */

        }
    }
}

void SystemProcess_Destroy(void)
{
    
}