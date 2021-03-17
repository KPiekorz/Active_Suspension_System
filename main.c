#include <stdio.h>
#include "accelerometer_manager.h"
#include "auto_model.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void main_PrintArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    // main_PrintArgs(argc, argv);
    // AutoModel_ModelState();

    // create basic pthread app
	pid_t pid;

	printf("I'm a child process talking to you from another program, pid = %d\n", getpid());
	printf("I'm a child process talking to you from another program, my parent pid = %d\n", getppid());

	printf("Type 'q' to quit both processes.\n");
	while(getc(stdin) == 'q') {}

	return EXIT_SUCCESS;
}

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