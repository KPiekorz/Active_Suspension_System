#include "control.h"
#include "system_utility.h"
#include "model_simulation.h"
#include "matrix_lib.h"

const char *control_fifo_name = "control_fifo";

#define MAX_CONTROL_FLOAT_DATA_LEN      20

/* MATRIX FOR CONTROLER K */

#define K_ROW_SIZE 1
#define K_COLUMN_SIZE 4

static float K_matrix[K_ROW_SIZE][K_COLUMN_SIZE];

static Mat K = {(float *)K_matrix, K_ROW_SIZE, K_COLUMN_SIZE};

#define GetK() (&K)

static void control_InitMatrixK(void)
{
    K_matrix[0][0] = 0;
    K_matrix[0][1] = 2300000;
    K_matrix[0][2] = 500000000;
    K_matrix[0][3] = 6000000;
}


/* MATRIX FOR SYSTEM STATES */

#define X_ROW_SIZE 4
#define X_COLUMN_SIZE 1

static float X_matrix[X_ROW_SIZE][X_COLUMN_SIZE];

static Mat X = {(float *)X_matrix, X_ROW_SIZE, X_COLUMN_SIZE};

#define GetX() (&X)

/* STATIC HELPER FUNCTION */

static void control_GetModelStates(float * float_data, int float_data_len, Mat * X)
{
    if (float_data_len >= 4)
    {
        set(X, 1, 1, float_data[0]);
        set(X, 2, 1, float_data[1]);
        set(X, 3, 1, float_data[2]);
        set(X, 4, 1, float_data[3]);
    }
}

static void control_CalculateAndSendControlForce(float * float_data, int float_data_len)
{
    /* here for now will be simple controller, in the future can be implemented more advanced controlers */

    // u = K * X

    /* init control K matrix */
    control_InitMatrixK();

    /* set model states */
    control_GetModelStates(float_data, float_data_len, GetX());

    /* calculate new control signale U (force) */
    Mat * U = multiply(GetK(), GetX());

    /* send control signal to model simulation process */

}

static void *control_ReceiveMessageThread(void *cookie)
{
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self());

    /* create message fifo queue */
    if (!SystemUtility_CreateMessageFifo(control_fifo_name))
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
        DELAY_MS(RECEIVE_FIFO_MESSAGE_SYSTEM_DELAY_MS);

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

        /* Init simulation of suspension */
        if (!SystemUtility_CreateThread(control_ReceiveMessageThread))
        {
            DEBUG_LOG_ERROR("[CONTROL] Control_Init, Can't create control receive thread thread!");
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

}

void Control_SendMessage(control_message_type_t message_type, float *data, int data_len)
{
    if (!SystemUtility_SendMessage(control_fifo_name, (int)message_type, data, data_len))
    {
        DEBUG_LOG_ERROR("[CONTROL] Control_SendMessage, Can't send message to gui process!");
    }
}