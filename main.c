#ifndef _POSIX_C_SOURCE
	#define _POSIX_C_SOURCE 200112L /* Or higher */
#endif

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "system_utility.h"
#include "system_process.h"
#include "sensors.h"
#include "gui.h"
#include "model_simulation.h"

/* static helper function prototype */
static void main_PrintArgs(int argc, char *argv[]);
static void main_InitSystemModules(void);

int main(int argc, char *argv[])
{
    // main_PrintArgs(argc, argv);
    // AutoModel_ModelState();

    // main_InitSystemModules();

    // Sensors_PrintSensorData();

    // Gui_RunGui();


    SystemProcess_Initialize();

	/* Wait for q(uit) */
  	while(getc(stdin)!='q') {}

    SystemProcess_Destroy();

	return EXIT_SUCCESS;
}

/**
 * @brief  Print program arguments entered by user.
 * @note
 * @param  argc:
 * @param  *argv[]:
 * @retval None
 */
static void main_PrintArgs(int argc, char *argv[])
{
    int i;
    if( argc >= 2 )
    {
        DEBUG_LOG_DEBUG("The arguments supplied are:");
        for(i = 1; i < argc; i++)
        {
            DEBUG_LOG_DEBUG("%s", argv[i]);
        }
    }
    else
    {
        DEBUG_LOG_DEBUG("Argument list is empty!");
    }
}

/**
 * @brief Init system modules, whcich is in declarated in modules array.
 * @note
 * @retval None
 */
static void main_InitSystemModules(void)
{

    int status = 0;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0); // this way, the father waits for all the child processes



    // old code maby will be useful
       /* create another process */
	// pid_t pid = fork();

    // if (0 == pid)
    // {
    //     /* child process */

    // }
    // else
    // {
    //     /* parent process */

    // }

	// while(getc(stdin) == 'q') {}

}