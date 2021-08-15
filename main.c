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

static void *main_KeyboardThread(void *cookie)
{
	SystemUtility_InitThread(pthread_self(), thread_priority_min);

	DEBUG_LOG_ALWAYS("[MAIN] main_KeyboardThread, Enter q to quit: ");
	while (getc(stdin) != 'q')
	{
	}

	raise(SIGINT);

	return 0;
}

static void main_ExitHandler(int sig)
{
	DEBUG_LOG_ALWAYS("[MAIN] main_ExitHandler, Quiting...");

	SystemProcess_Destroy();

    exit(0);
}

int main(int argc, char *argv[])
{
	SystemProcess_Initialize();

	/* Init simulation of suspension */
	if (!SystemUtility_CreateThread(main_KeyboardThread))
	{
		DEBUG_LOG_ERROR("[MAIN] main, Can't create keyboard thread!");
	}

    sigset_t mask;
	SignalsHandler_InitSignalMask(&mask);

	if (!SignalsHandler_RegisterSignalHandler(SIGINT, main_ExitHandler))
	{
		DEBUG_LOG_ERROR("[MAIN] main, Can't register SIGINT signal!");
	}

	if (!SignalsHandler_RegisterSignalHandler(SIGTSTP, main_ExitHandler))
	{
		DEBUG_LOG_ERROR("[MAIN] main, Can't register SIGINT signal!");
	}

	while (1)
	{
		SignalsHandler_HandleSignals(&mask);
	}

	return EXIT_SUCCESS;
}

