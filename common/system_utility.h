#ifndef SYSTEM_UTILITY_H
#define SYSTEM_UTILITY_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/*** TIME TRANSFORMATION ***/

#define SEC_TO_US(s)    (s * 1000000)
#define SEC_TO_US(s)    (s * 1000000)

/*** DELAY MACROS ***/

#define Delay_s(s)    (usleep(SEC_TO_US(s)))
#define Delay_ms(s)    (usleep(SEC_TO_US(s)))

/*** DEBUG LOGS ***/

#define ENABLE_VERBOSE_LOG
#define ENABLE_DEBUG_LOG
#define ENABLE_WARN_LOG
#define ENABLE_ERROR_LOG

#ifdef ENABLE_VERBOSE_LOG
    #define DEBUG_LOG_VERBOSE(...)  { printf("VERBOSE: " __VA_ARGS__);  printf("\n"); }
#else
    #define DEBUG_LOG_VERBOSE(...) // nothing
#endif

#ifdef ENABLE_DEBUG_LOG
    #define DEBUG_LOG_DEBUG(...)    { printf("DEBUG: " __VA_ARGS__);    printf("\n"); }
#else
    #define DEBUG_LOG_DEBUG(...) // nothing
#endif

#ifdef ENABLE_WARN_LOG
    #define DEBUG_LOG_WARN(...)     { printf("WARN: " __VA_ARGS__);     printf("\n"); }
#else
    #define DEBUG_LOG_WARN(...)  // nothing
#endif

#ifdef ENABLE_ERROR_LOG
    #define DEBUG_LOG_ERROR(...)    { printf("ERROR: " __VA_ARGS__);    printf("\n"); }
#else
    #define DEBUG_LOG_ERROR(...)  // nothing
#endif

#endif  /* SYSTEM_UTILITY_H */
