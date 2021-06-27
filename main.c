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

    // printf("Delay in us: %d\n\r", SEC_TO_US(1));

    // Gui_RunGui();



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
        printf("The arguments supplied are:\n");
        for(i = 1; i < argc; i++)
        {
            printf("%s\n", argv[i]);
        }
    }
    else
    {
        printf("Argument list is empty!\n");
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

    //Parent code (After all child processes end)
    printf("System end!!!");

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

	// printf("I'm a child process talking to you from another program, pid = %d\n", getpid());
	// printf("I'm a child process talking to you from another program, my parent pid = %d\n", getppid());

	// printf("Type 'q' to quit both processes.\n");
	// while(getc(stdin) == 'q') {}

}