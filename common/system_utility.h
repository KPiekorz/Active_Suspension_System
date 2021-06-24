#ifndef SYSTEM_UTILITY_H
#define SYSTEM_UTILITY_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define SEC_TO_US(s)    (s * 1000000)

#define Delay(s)    (usleep(SEC_TO_US(s)))

#endif  
