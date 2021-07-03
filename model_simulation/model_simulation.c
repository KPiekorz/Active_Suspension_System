#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

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
        const char * message = "Hello gui!";
        Gui_SendMessage(gui_message_control_signal, message, strlen(message));

        DELAY_S(3);
        DEBUG_LOG_VERBOSE("[SIM] Model simulation process running...");
    }

    exit(EXIT_SUCCESS);
}

void ModelSimulation_Destroy(void)
{
    DEBUG_LOG_DEBUG("Destroy model simulation process...");

}
