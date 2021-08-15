#ifndef SIGNALS_HANDLER_H
#define SIGNALS_HANDLER_H

#include <signal.h>

void SignalsHandler_RegisterSignalHandler(int signal, __sighandler_t handler);

void SignalsHandler_InitSignalMask(sigset_t * mask);

void SignalsHandler_HandleSignals(sigset_t * mask);

#endif  /* SIGNALS_HANDLER_H */