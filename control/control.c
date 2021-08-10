#include "control.h"
#include "system_utility.h"
#include "model_simulation.h"
#include "matrix_lib.h"
#include "PID.h"

/*** fifo for control process ***/

const char *control_fifo_name = "control_fifo";

#define MAX_CONTROL_FLOAT_DATA_LEN      20

/* PID CONTROLLER PARAMETERS */

#define PID_KP  2000
#define PID_KI  10
#define PID_KD  0

#define PID_TAU 0.02f

#define PID_LIM_MIN -100000.0f
#define PID_LIM_MAX  100000.0f

#define PID_LIM_MIN_INT -100000.0f
#define PID_LIM_MAX_INT  100000.0f

PIDController pid = { PID_KP, PID_KI, PID_KD, PID_TAU, PID_LIM_MIN, PID_LIM_MAX, PID_LIM_MIN_INT, PID_LIM_MAX_INT, SAMPLE_TIME };

/* MATRIX FOR CONTROLER K */

#define K_ROW_SIZE 1
#define K_COLUMN_SIZE 4

static float K_matrix[K_ROW_SIZE][K_COLUMN_SIZE];

static Mat K = {(float *)K_matrix, K_ROW_SIZE, K_COLUMN_SIZE};

#define GetK() (&K)

static void control_InitMatrixK(void)
{
    K_matrix[0][0] = 0;
    K_matrix[0][1] = 230;
    K_matrix[0][2] = 5000;
    K_matrix[0][3] = 0;
    K_matrix[0][4] = 800;
}


/* MATRIX FOR SYSTEM STATES */

#define X_ROW_SIZE 5
#define X_COLUMN_SIZE 1

static float X_matrix[X_ROW_SIZE][X_COLUMN_SIZE];

static Mat X = {(float *)X_matrix, X_ROW_SIZE, X_COLUMN_SIZE};

#define GetX() (&X)

/* STATIC HELPER FUNCTION */

static void control_GetModelStates(float * float_data, int float_data_len, Mat * X)
{
    if (float_data_len >= X_ROW_SIZE)
    {
        set(X, 1, 1, float_data[MODEL_SIMULATION_STATE_X1]);
        set(X, 2, 1, float_data[MODEL_SIMULATION_STATE_X1_DOT]);
        set(X, 3, 1, float_data[MODEL_SIMULATION_STATE_Y1]);
        set(X, 4, 1, float_data[MODEL_SIMULATION_STATE_Y1_DOT]);
        set(X, 5, 1, float_data[MODEL_SIMULATION_STATE_Y2]);
    }
}

static int control_SetControlForce(float force, float * float_data, int float_data_len)
{
    if (float_data_len >= 1)
    {
        float_data[0] = force;

        return 1;
    }

    return 0;
}

static void control_FeedforwrdControler(float * float_data, int float_data_len)
{
    /* here for now will be simple controller, in the future can be implemented more advanced controlers */

    // u = K * X

    /* init control K matrix */
    control_InitMatrixK();

    /* get model states */
    control_GetModelStates(float_data, float_data_len, GetX());

    /* calculate new control signal U (force) */
    Mat * U = multiply(GetK(), GetX());

    float force_data[MAX_CONTROL_FLOAT_DATA_LEN];
    int force_data_len = control_SetControlForce(get(U, 1, 1), force_data, MAX_CONTROL_FLOAT_DATA_LEN);

    /* send control signal to model simulation process */
    ModelSimulation_SendMessage(simulation_message_control_force, force_data, force_data_len);
}

static void control_PIDControler(float * float_data, int float_data_len)
{
    /* get model states */
    control_GetModelStates(float_data, float_data_len, GetX());

    /* init pid controller */
    PIDController_Init(&pid);

    /* Compute new control signal */
    float setpoint = 0;
    float measurement = get(GetX(), 3, 1);

    PIDController_Update(&pid, setpoint, measurement);

    float force_data[MAX_CONTROL_FLOAT_DATA_LEN];
    int force_data_len = control_SetControlForce(pid.out, force_data, MAX_CONTROL_FLOAT_DATA_LEN);

    /* send control signal to model simulation process */
    ModelSimulation_SendMessage(simulation_message_control_force, force_data, force_data_len);
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

                    #ifdef FEEDFORWARD_CONTRLER
                        control_FeedforwrdControler(float_data, float_data_len);
                    #endif

                    #ifdef PID_CONTROLLER
                        control_PIDControler(float_data, float_data_len);
                    #endif

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
		    DELAY_S(SYSTEM_EVENT_LOOP_DELAY_S);
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