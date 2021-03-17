#include <stdio.h>
#include "accelerometer_manager.h"
#include "auto_model.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* modules init functions */
typedef void (*module_init)(void);

typedef struct
{
    module_init module_init_func;
} module_init_t;

const module_init_t system_module_init[] = 
{
    {AutoModel_Init},
};

#define GetModulesCount()   (sizeof(system_module_init)/sizeof(module_init_t))    

/* static helper function prototype */
static void main_PrintArgs(int argc, char *argv[]);
static void main_InitSystemModules(void);

int main(int argc, char *argv[])
{
    // main_PrintArgs(argc, argv);
    // AutoModel_ModelState();

    /* create another process */
	pid_t pid = fork();

    if (0 == pid)
    {
        /* child process */
        
    }
    else
    {
        /* parent process */

    }

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

/**
 * Init system modules, whcich is in declarated in modules array.
 */
static void main_InitSystemModules(void)
{
    //Parent code (Before all child processes start)


    for (int i = 0; i < GetModulesCount(); i++)
    {
        pid_t child_pid = fork();
        if (0 == child_pid) 
        {
            /* child code */
            system_module_init[i].module_init_func();

            // it is child process now and before that all threads within process should be joined and after that process can exit(0) (with code zero = mean success)
            exit(EXIT_SUCCESS);
        }
    }
    
    int status = 0;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0); // this way, the father waits for all the child processes 

    //Parent code (After all child processes end)

}