#include "control.h"
#include "system_utility.h"
#include "model_simulation.h"
#include "matrix_lib.h"

const char *control_fifo_name = "simulation_fifo";

#define MAX_CONTROL_FLOAT_DATA_LEN      20

/* STATIC HELPER FUNCTION */

static void control_CalculateAndSendControlForce(float * float_data, int float_data_len)
{
    /* here for now will be simple controller, in the future can be implemented more advanced controlers */

}

static void *control_ReceiveMessageThread(void *cookie)
{
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self());

    /* create message fifo queue */
    if (!SystemUtility_CreateMessageFifo(control_fifo_name) == false)
    {
        DEBUG_LOG_ERROR("[CONTROL] control_ReceiveMessageThread, Can't create FIFO!");
        return 0;
    }

    control_message_type_t message_type = control_message_unknown;
    int float_data_len = MAX_CONTROL_FLOAT_DATA_LEN;
    float float_data[MAX_CONTROL_FLOAT_DATA_LEN];

    while (true)
    {
        /* delay for waiting for another message */
        DELAY_MS(1);

        /* try receive  */
        float_data_len = MAX_CONTROL_FLOAT_DATA_LEN;
        if (true == SystemUtility_ReceiveMessage(control_fifo_name, (int *)&message_type, float_data, &float_data_len))
        {
            switch (message_type)
            {
                case control_message_model_states:
                    control_CalculateAndSendControlForce(float_data, float_data_len);
                break;
                default:
                break;
            }

            // for (int i = 0; i < float_data_len; i++)
            // {
            //     DEBUG_LOG_VERBOSE("[CONTROL] control_ReceiveMessageThread, float[%d]: %f", i, float_data[i]);
            // }
        }
    }

    return 0;
}

/* GLOBAL FUNCTION */

void Control_Init(void)
{
    #ifdef INIT_CONTROL
        DEBUG_LOG_DEBUG("[CONTROL] Control_Init, Init model simulation process...");

        /* Init simulation of suspension */
        if (!SystemUtility_CreateThread(control_ReceiveMessageThread))
        {
            DEBUG_LOG_ERROR("[CONTROL] Control_Init, Can't create simulaton step thread!");
        }

        while (1)
        {
		    DELAY_S(5);
        }

        exit(EXIT_SUCCESS);
    #else
        DEBUG_LOG_INFO("[CONTROL] Control_Init, Won't be initialized.");
    #endif
}

void Control_Destroy(void)
{
    DEBUG_LOG_DEBUG("[CONTROL] Control_Destroy, Destroy CONTROL process...");

}

void Control_SendMessage(control_message_type_t message_type, float *data, int data_len)
{
    if (!SystemUtility_SendMessage(control_fifo_name, (int)message_type, data, data_len))
    {
        DEBUG_LOG_ERROR("[CONTROL] ModelSimulation_SendMessage, Can't send message to gui process!");
    }
}