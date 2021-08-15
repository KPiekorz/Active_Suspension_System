#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>

#include "system_process.h"
#include "gui.h"
#include "control.h"
#include "model_simulation.h"
#include "sensors.h"
#include "system_utility.h"

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

process_attributes_t_t system_process_attributes[] =
{
    {Gui_Init,              Gui_Destroy,                UNKNOWN_PID,        "Gui"                    },
    {Control_Init,          Control_Destroy,            UNKNOWN_PID,        "Control"                },
    {ModelSimulation_Init,  ModelSimulation_Destroy,    UNKNOWN_PID,        "Model_simulation"       },
    {Sensor_Init,           Sensor_Destroy,             UNKNOWN_PID,        "Sensors"                },
};

#define GetProcessCount()   ((int)(sizeof(system_process_attributes)/sizeof(process_attributes_t_t)))

/*** STATIC FUNCTION ***/

static void systemProcess_KillAllSystemProcess(void)
{
    for (int i = 0; i < GetProcessCount(); i++)
    {
        DEBUG_LOG_INFO("systemProcess_KillAllSystemProcess, process name: %s, pid: %d", system_process_attributes[i].process_name, system_process_attributes[i].process_pid);
        kill(system_process_attributes[i].process_pid, SIGKILL);
        system_process_attributes[i].process_pid = UNKNOWN_PID;
    }
}

static void systemProcess_PrintAllProcessPid(void)
{
    for (int i = 0; i < GetProcessCount(); i++)
    {
        DEBUG_LOG_INFO("Process %s, pid: %d", system_process_attributes[i].process_name,
                                               system_process_attributes[i].process_pid);
    }
}

/*** GLOBAL FUNCTION ***/

void SystemProcess_Initialize(void)
{
    DEBUG_LOG_INFO("SystemProcess_Initialize");

    for (int i = 0; i < GetProcessCount(); i++)
    {
        /* here we will create new process */
        pid_t child_pid = fork();

        if (0 == child_pid)
        {
            /* child code */
            system_process_attributes[i].process_init_func();

            exit(EXIT_SUCCESS);
        }
        else
        {
            system_process_attributes[i].process_pid = child_pid;
        }
    }

    /* print all created system process */
    systemProcess_PrintAllProcessPid();
}

void SystemProcess_Destroy(void)
{
    DEBUG_LOG_INFO("SystemProcess_Destroy");

    for (int i = 0; i < GetProcessCount(); i++)
    {
        system_process_attributes[i].process_destroy_func();
    }

    /* kill all system process */
    systemProcess_KillAllSystemProcess();
}
