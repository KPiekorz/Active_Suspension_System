#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>

#include "model_simulation.h"
#include "gui.h"
#include "system_utility.h"
#include "control.h"
#include "sensors.h"

/*** STATIC FUNCTION ***/

static void modelSimluation_StartSimulation(void)
{
    // here will be copied model simulation from simulink
}

/*** GLOBAL FUNCTION ***/

void ModelSimulation_Init(void)
{
    DEBUG_LOG_DEBUG("Init model simulation process...");

    while (1)
    {
        DEBUG_LOG_VERBOSE("Model simulation process running...");
		usleep(SEC_TO_US(3));
    }

    exit(EXIT_SUCCESS);
}

void ModelSimulation_Destroy(void)
{
    DEBUG_LOG_DEBUG("Destroy model simulation process...");

}
