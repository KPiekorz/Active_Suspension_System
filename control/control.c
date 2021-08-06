#include "control.h"
#include "system_utility.h"
#include "model_simulation.h"

const char *control_fifo_name = "simulation_fifo";

/* STATIC HELPER FUNCTION */

static void *modelSimluation_ReceiveMessageThread(void *cookie)
{
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self());

    /* create message fifo queue */
    if (!SystemUtility_CreateMessageFifo(control_fifo_name) == false)
    {
        DEBUG_LOG_ERROR("[SIM] modelSimluation_ReceiveMessageThread, Can't create FIFO!");
        return 0;
    }

    // gui_message_type_t message_type = simulation_message_unknown;
    // int float_data_len = MAX_SIMULATION_FLOAT_DATA_LEN;
    // float float_data[MAX_SIMULATION_FLOAT_DATA_LEN];

    // while (true)
    // {
    //     /* delay for waiting for another message */
    //     DELAY_MS(1);

    //     /* try receive  */
    //     float_data_len = MAX_SIMULATION_FLOAT_DATA_LEN;
    //     if (true == SystemUtility_ReceiveMessage(simulation_fifo_name, (int *)&message_type, float_data, &float_data_len))
    //     {
    //         switch (message_type)
    //         {
    //             case simulation_message_control_force:
    //                 pthread_mutex_lock(GetForceMutex());

    //                 if (float_data_len = 1)
    //                 {
    //                     force =+ float_data[0]; // last control force value add to last force value
    //                 }

    //                 pthread_mutex_unlock(GetForceMutex());
    //             break;
    //             default:
    //             break;
    //         }

    //         for (int i = 0; i < float_data_len; i++)
    //         {
    //             DEBUG_LOG_VERBOSE("[SIM] modelSimluation_ReceiveMessageThread, float[%d]: %f", i, float_data[i]);
    //         }
    //     }
    // }

    return 0;
}

/* GLOBAL FUNCTION */

void Control_Init(void)
{
    #ifdef INIT_CONTROL
        DEBUG_LOG_DEBUG("Control_Init, Init model simulation process...");

        while (1)
        {
            ModelSimulation_SendMessage(model_simulation_message_control_force, NULL, 0);

            DEBUG_LOG_VERBOSE("Control_Init, Control process running...");
		    DELAY_S(2);
        }

        exit(EXIT_SUCCESS);
    #else
        DEBUG_LOG_INFO("[SIM] Control_Init, Won't be initialized.");
    #endif
}

void Control_Destroy(void)
{
    DEBUG_LOG_DEBUG("Control_Destroy, Destroy CONTROL process...");

}

void Control_SendMessage(control_message_type_t message_type, float *data, int data_len)
{
    if (!SystemUtility_SendMessage(control_fifo_name, (int)message_type, data, data_len))
    {
        DEBUG_LOG_ERROR("[CONTROL] ModelSimulation_SendMessage, Can't send message to gui process!");
    }
}