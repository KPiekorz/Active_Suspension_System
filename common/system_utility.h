#ifndef SYSTEM_UTILITY_H
#define SYSTEM_UTILITY_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/*** SYSTEM_FIFO_SIZE ***/

#define DEFAULT_FIFO_SIZE                   (256)

/*** SYSTEM MESSAGE OFFSETS ***/

#define SYSTEM_MESSAGE_TYPE_OFFSET          (0)
#define SYSTEM_MESSAGE_LENGTH_OFFSET        (1)
#define SYSTEM_MESSAGE_DATA_OFFSET          (2)
#define GET_MESSAGE_SIZE(l)                 (l + 2)

/*** TIME TRANSFORMATION ***/

#define SEC_TO_US(s)                        (s * 1000000)
#define MS_TO_US(s)                         (s * 1000)

/*** DELAY MACROS ***/

#define DELAY_S(s)                          (usleep(SEC_TO_US(s)))
#define DELAY_MS(s)                         (usleep(MS_TO_US(s)))

/*** DEBUG LOGS ***/

#define ENABLE_VERBOSE_LOG
#define ENABLE_INFO_LOG
#define ENABLE_DEBUG_LOG
#define ENABLE_WARN_LOG

#define DEBUG_LOG_ALWAYS(...)       { printf("ALWAYS: " __VA_ARGS__);   printf("\n"); }

#ifdef ENABLE_INFO_LOG
    #define DEBUG_LOG_INFO(...)     { printf("INFO: " __VA_ARGS__);     printf("\n"); }
#else
    #define DEBUG_LOG_INFO(...)    // nothing
#endif

#ifdef ENABLE_DEBUG_LOG
    #define DEBUG_LOG_DEBUG(...)    { printf("DEBUG: " __VA_ARGS__);    fflush(stdout); printf("\n"); fflush(stdout); }
#else
    #define DEBUG_LOG_DEBUG(...)    // nothing
#endif

#ifdef ENABLE_VERBOSE_LOG
    #define DEBUG_LOG_VERBOSE(...)  { printf("VERBOSE: " __VA_ARGS__);  fflush(stdout); printf("\n"); fflush(stdout); }
#else
    #define DEBUG_LOG_VERBOSE(...)  // nothing
#endif

#ifdef ENABLE_WARN_LOG
    #define DEBUG_LOG_WARN(...)     { printf("WARN: " __VA_ARGS__);     fflush(stdout); printf("\n"); fflush(stdout); }
#else
    #define DEBUG_LOG_WARN(...)     // nothing
#endif

#define DEBUG_LOG_ERROR(...)        { fprintf(stderr, "ERROR: " __VA_ARGS__);    printf("\n"); }

#endif  /* SYSTEM_UTILITY_H */
