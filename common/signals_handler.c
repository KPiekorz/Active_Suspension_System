#include "signals_handler.h"

/*** STATIC FUNCTION ***/

/*** GLOBAL FUNCTION ***/

void SignalsHandler_RegisterSignalHandler(int signal, __sighandler_t handler)
{

}

void SignalsHandler_InitSignalMask(sigset_t * mask)
{

}

void SignalsHandler_HandleSignals(sigset_t * mask)
{
    sigsuspend(mask);
}