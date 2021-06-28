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

int main(int argc, char *argv[])
{
    main_PrintArgs(argc, argv);

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