#ifndef _POSIX_C_SOURCE
	#define _POSIX_C_SOURCE 200112L /* Or higher */
#endif

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "model_simulation.h"
#include "system_utility.h"
#include "system_process.h"
#include "signals_handler.h"

#include "gui.h"

int main(int argc, char *argv[])
{
    SystemProcess_Initialize();

    DEBUG_LOG_ALWAYS("Enter q to quit: ");
  	while(getc(stdin)!='q') {}

    SystemProcess_Destroy();

	return EXIT_SUCCESS;
}