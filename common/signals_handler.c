#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include "signals_handler.h"

/*** STATIC FUNCTION ***/

/*** GLOBAL FUNCTION ***/

bool SignalsHandler_RegisterSignalHandler(int signal, __sighandler_t handler)
{
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    /* Register signal handler for SIGINT */
    if (sigaction(signal, &action, NULL) < 0)
    {
        return false;
    }

    return true;
}

void SignalsHandler_InitSignalMask(sigset_t * mask)
{
    sigemptyset(mask);
    sigprocmask(0, NULL, mask);

    for (int i = 2; i < SIGRTMAX; i++)
    {
        sigaddset(mask, SIGRTMIN+i);
    }

    sigprocmask(SIG_SETMASK, mask, NULL);
}

void SignalsHandler_HandleSignals(sigset_t * mask)
{
    sigsuspend(mask);
}