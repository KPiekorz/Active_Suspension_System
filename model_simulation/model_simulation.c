#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "system_utility.h"
#include "model_simulation.h"
#include "gui.h"
#include "control.h"
#include "sensors.h"
#include "matrix_lib.h"

typedef enum
{
    road_type_step,
    road_type_random,
    road_type_impuls,
    road_type_ramp,

} road_type_t;

/*** INCLUDE CONTROLER ***/

#define INCLUDE_CONTROLER

/*** MODEL_SIMULATION_INTERVAL_STEP ***/

#define MODEL_SIMULATION_STEP_INTERVAL_MS       (100)

/*** MODEL SIMULATION MESSAGE FIFO ***/

#define MAX_SIMULATION_FLOAT_DATA_LEN           (5)

const char *simulation_fifo_name = "simulation_fifo";

pthread_mutex_t mutex_force = PTHREAD_MUTEX_INITIALIZER;
#define GetForceMutex()   (&mutex_force)

#define DEFAULT_VALUE (0)

/*** SIMULATION PARAMETERS ***/

#define SIM_TIME (50)
const float simulation_time = SIM_TIME; // how many iteration will be performed
const float sampling_period = 0.5;

#define STEP_VALUE  0.1
#define ST_TIME     10
const float step_time = ST_TIME;

#define IMPULSE_VALUE   1000
#define IM_TIME         40
const float impulse_time = IM_TIME;

/**
 * @note Road and force will be set in input matrix in every interation
 */

/*** INPUT CONTROL SIGNAL - FORCE ***/

#define INITIAL_FORCE_VALUE     (-5)
static float force = INITIAL_FORCE_VALUE; // this varialbe will be update from control process (access to force variable have to be done through mutex semaphore)

/*** INPUT ROAD SIGNAL ***/

static float road[SIM_TIME]; // for now it will be only step signal, in 10 step in will be 10 [cm] (0.1 [m])

static void modelSimluation_GenerateStep(void)
{
    if (step_time >= simulation_time)
    {
        DEBUG_LOG_ERROR("[SIM] modelSimluation_GenerateStep, invalid step time!");
        return;
    }

    // before step
    for (int i = 0; i < step_time; i++)
    {
        road[i] = 0;
    }

    // after step
    for (int i = step_time; i < simulation_time; i++)
    {
        road[i] = STEP_VALUE;
    }
}

static void modelSimluation_GenerateRandom(void)
{
    for (int i = 0; i < simulation_time; i++)
    {
        // road[i] = ;
    }
}

static void modelSimluation_GenerateImpuls(void)
{
    if (impulse_time >= (simulation_time/2))
    {
        DEBUG_LOG_ERROR("[SIM] modelSimluation_GenerateStep, invalid step time!");
        return;
    }

    // before step
    for (int i = 0; i < impulse_time; i++)
    {
        road[i] = 0;
    }

    road[IM_TIME] = IMPULSE_VALUE;

    // after step
    for (int i = (impulse_time+1); i < simulation_time; i++)
    {
        road[i] = 0;
    }
}

static void  modelSimluation_GenerateRamp(void)
{

}

static void modelSimluation_GenerateRoad(road_type_t type)
{
    switch (type)
    {
    case road_type_step:
        modelSimluation_GenerateStep();
    break;
    case road_type_random:
        modelSimluation_GenerateRandom();
    break;
    case road_type_impuls:
        modelSimluation_GenerateImpuls();
    break;
    case road_type_ramp:
        modelSimluation_GenerateRamp();
    break;
    default:
        DEBUG_LOG_WARN("[SIM] modelSimluation_GenerateRoad, unknown road!");
        break;
    }
}

/*** VARIABLES ***/

const float m1 = 2500;
const float m2 = 320;
const float k1 = 80000;
const float k2 = 500000;
const float b1 = 350;
const float b2 = 15020;

/*** STATE SPACE MODEL - A ***/

#define A_ROW_SIZE 4
#define A_COLUMN_SIZE 4

static float A_matrix[A_ROW_SIZE][A_COLUMN_SIZE];

static Mat A = {(float *)A_matrix, A_ROW_SIZE, A_ROW_SIZE};

#define GetA() (&A)

static void modelSimluation_InitMatrixA(void)
{
    A_matrix[0][0] = 0;
    A_matrix[0][1] = 1;
    A_matrix[0][2] = 0;
    A_matrix[0][3] = 0;

    A_matrix[1][0] = -(b1 * b2) / (m1 * m2);
    A_matrix[1][1] = 0;
    A_matrix[1][2] = ((b1 / m1) * ((b1 / m1) + (b1 / m2) + (b2 / m2))) - (k1 / m1);
    A_matrix[1][3] = -(b1 / m1);

    A_matrix[2][0] = b2 / m2;
    A_matrix[2][1] = 0;
    A_matrix[2][2] = -((b1 / m1) + (b1 / m2) + (b2 / m2));
    A_matrix[2][3] = 1;

    A_matrix[3][0] = k2 / m2;
    A_matrix[3][1] = 0;
    A_matrix[3][2] = -((k1 / m1) + (k1 / m2) + (k2 / m2));
    A_matrix[3][3] = 0;
}

/*** STATE SPACE MODEL - B ***/

#define B_ROW_SIZE 4
#define B_COLUMN_SIZE 2

static float B_matrix[B_ROW_SIZE][B_COLUMN_SIZE];

static Mat B = {(float *)B_matrix, B_ROW_SIZE, B_COLUMN_SIZE};

#define GetB() (&B)

static void modelSimluation_InitMatrixB(void)
{
    B_matrix[0][0] = 0;
    B_matrix[0][1] = 0;

    B_matrix[1][0] = 1 / m1;
    B_matrix[1][1] = (b1 * b2) / (m1 * m2);

    B_matrix[2][0] = 0;
    B_matrix[2][1] = -(b2 / m2);

    B_matrix[3][0] = (1 / m1) + (1 / m2);
    B_matrix[3][1] = -(k2 / m2);
}

/*** STATE SPACE MODEL - C ***/

#define C_ROW_SIZE 1
#define C_COLUMN_SIZE 4

static float C_matrix[C_ROW_SIZE][C_COLUMN_SIZE];

static Mat C = {(float *)C_matrix, C_ROW_SIZE, C_COLUMN_SIZE};

#define GetC() (&C)

static void modelSimluation_InitMatrixC(void)
{
    C_matrix[0][0] = 0;
    C_matrix[0][1] = 0;
    C_matrix[0][2] = 1;
    C_matrix[0][3] = 0;
}

/*** STATE SPACE MODEL - STATES ***/

#define INITIAL_STATES_ROW_SIZE 4
#define INITIAL_STATES_COLUMN_SIZE 1

static float INITIAL_STATES_matrix[INITIAL_STATES_ROW_SIZE][INITIAL_STATES_COLUMN_SIZE];

static Mat INITIAL_STATES = {(float *)INITIAL_STATES_matrix, INITIAL_STATES_ROW_SIZE, INITIAL_STATES_COLUMN_SIZE};

#define GetINITIAL_STATES() (&INITIAL_STATES)

static void modelSimluation_InitMatrixINITIAL_STATES(void)
{
    INITIAL_STATES_matrix[0][0] = 0;
    INITIAL_STATES_matrix[0][1] = 0;
    INITIAL_STATES_matrix[0][2] = 0;
    INITIAL_STATES_matrix[0][3] = 0;
}

/*** INPUT MATRIX ***/

#define INPUT_ROW_SIZE 2
#define INPUT_COLUMN_SIZE 1

/*** STATIC FUNCTION ***/

static void modelSimluation_SendModelStates(Mat *x, float road, int iteration)
{
    if (x->row == 4 && x->col == 1)
    {
        pthread_mutex_lock(GetForceMutex());

        float model_simulation_data[GUI_MODEL_SIMULATION_DATA_SIZE];
        model_simulation_data[0] = get(x, 1, 1);
        model_simulation_data[1] = get(x, 2, 1);
        model_simulation_data[2] = get(x, 3, 1);
        model_simulation_data[3] = get(x, 4, 1);
        model_simulation_data[4] = road;
        model_simulation_data[5] = force;
        model_simulation_data[6] = (float)iteration;

        pthread_mutex_unlock(GetForceMutex());

        Gui_SendMessage(gui_message_model_simulation_data, model_simulation_data, GUI_MODEL_SIMULATION_DATA_SIZE);
        Control_SendMessage(control_message_model_states, model_simulation_data, GUI_MODEL_SIMULATION_DATA_SIZE);
    }
    else
    {
        DEBUG_LOG_ERROR("[SIM] modelSimluation_SendModelStates, wrong matrix!!!");
    }
}

static void *modelSimluation_SimulationStepThread(void *cookie)
{
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self());

    modelSimluation_InitMatrixA();
    modelSimluation_InitMatrixB();
    modelSimluation_InitMatrixC();

    // calcualte Ad matrix - discrete matric of A
    Mat *s_a = scalermultiply(GetA(), sampling_period);
    Mat *I = eye(A_ROW_SIZE);
    Mat *before_inv = minus(I, s_a);
    freemat(s_a);
    freemat(I);
    Mat *Ad = inverse(before_inv);
    freemat(before_inv);

    // calcualte Bd matrix - discrete matric of B
    Mat *ad_s = scalermultiply(Ad, sampling_period);
    Mat *Bd = multiply(ad_s, GetB());
    freemat(ad_s);

    // x(k-1)
    Mat *xk_1 = newmat(INITIAL_STATES_ROW_SIZE, INITIAL_STATES_COLUMN_SIZE, DEFAULT_VALUE);

    DELAY_S(1);

    // actual simulation
    for (int i = 0; i < simulation_time; i++)
    {
        if (i == 0)
        {
            pthread_mutex_lock(GetForceMutex());

            // set input matrix
            Mat *INPUT = newmat(INPUT_ROW_SIZE, INPUT_COLUMN_SIZE, DEFAULT_VALUE);
            set(INPUT, 1, 1, road[i]);
            set(INPUT, 2, 1, force); // this will be update by mesage from control process

            pthread_mutex_unlock(GetForceMutex());

            // send states to control and gui process (Xd and Yd)
            modelSimluation_SendModelStates(GetINITIAL_STATES(), road[i], i);

            // calculate x
            Mat *a_x = multiply(Ad, GetINITIAL_STATES());
            Mat *b_i = multiply(Bd, INPUT);

            Mat *xk = sum(a_x, b_i);
            set(xk_1, 1, 1, get(xk, 1, 1));
            set(xk_1, 2, 1, get(xk, 2, 1));
            set(xk_1, 3, 1, get(xk, 3, 1));
            set(xk_1, 4, 1, get(xk, 4, 1));

            freemat(INPUT);
            freemat(a_x);
            freemat(b_i);
            freemat(xk);
        }
        else
        {
            pthread_mutex_lock(GetForceMutex());

            // set input matrix
            Mat *INPUT = newmat(INPUT_ROW_SIZE, INPUT_COLUMN_SIZE, DEFAULT_VALUE);
            set(INPUT, 1, 1, road[i]);
            set(INPUT, 2, 1, force); // this will be update by mesage from control process

            showmat(INPUT);

            pthread_mutex_unlock(GetForceMutex());

            // send states to control and gui process (Xd and Yd)
            modelSimluation_SendModelStates(xk_1, road[i], i);

            // calculate x
            Mat *a_x = multiply(Ad, xk_1);
            Mat *b_i = multiply(Bd, INPUT);

            Mat *xk = sum(a_x, b_i);
            set(xk_1, 1, 1, get(xk, 1, 1));
            set(xk_1, 2, 1, get(xk, 2, 1));
            set(xk_1, 3, 1, get(xk, 3, 1));
            set(xk_1, 4, 1, get(xk, 4, 1));

            freemat(INPUT);
            freemat(a_x);
            freemat(b_i);
            freemat(xk);
        }

        DELAY_MS(MODEL_SIMULATION_STEP_INTERVAL_MS);
    }

    // send states to control and gui process (Xd and Yd)
    modelSimluation_SendModelStates(xk_1, road[SIM_TIME-1], (int)SIM_TIME-1);

    freemat(Ad);
    freemat(Bd);
    freemat(xk_1);

    return 0;
}

static void *modelSimluation_ReceiveMessageThread(void *cookie)
{
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self());

    /* create message fifo queue */
    if (!SystemUtility_CreateMessageFifo(simulation_fifo_name))
    {
        DEBUG_LOG_ERROR("[SIM] modelSimluation_ReceiveMessageThread, Can't create FIFO!");
        return 0;
    }

    gui_message_type_t message_type = simulation_message_unknown;
    int float_data_len = MAX_SIMULATION_FLOAT_DATA_LEN;
    float float_data[MAX_SIMULATION_FLOAT_DATA_LEN];

    while (true)
    {
        /* delay for waiting for another message */
        DELAY_MS(RECEIVE_FIFO_MESSAGE_SYSTEM_DELAY_MS);

        /* try receive  */
        float_data_len = MAX_SIMULATION_FLOAT_DATA_LEN;
        if (true == SystemUtility_ReceiveMessage(simulation_fifo_name, (int *)&message_type, float_data, &float_data_len))
        {
            switch (message_type)
            {
                case simulation_message_control_force:
                    pthread_mutex_lock(GetForceMutex());

                    if (float_data_len = 1)
                    {
                        #ifdef INCLUDE_CONTROLER
                            force += float_data[0]; // last control force value add to last force value
                        #else
                            force += 0; // last control force value add to last force value
                        #endif /* INCLUDE_CONTROLER */
                    }

                    pthread_mutex_unlock(GetForceMutex());
                break;
                default:
                break;
            }

            // for (int i = 0; i < float_data_len; i++)
            // {
            //     DEBUG_LOG_VERBOSE("[SIM] modelSimluation_ReceiveMessageThread, float[%d]: %f", i, float_data[i]);
            // }
        }
    }

    return 0;
}

/*** GLOBAL FUNCTION ***/

void ModelSimulation_Init(void)
{
#ifdef INIT_MODEL_SIMULATION

    /* Init road input */
    modelSimluation_GenerateRoad(road_type_step);

    /* Init simulation of suspension */
    if (!SystemUtility_CreateThread(modelSimluation_SimulationStepThread))
    {
        DEBUG_LOG_ERROR("[SIM] ModelSimulation_Init, Can't create simulaton step thread!");
    }

    /* Init receive message */
    if (!SystemUtility_CreateThread(modelSimluation_ReceiveMessageThread))
    {
        DEBUG_LOG_ERROR("[SIM] ModelSimulation_Init, Can't create receive thread!");
    }

    while (1)
    {
        DELAY_S(SYSTEM_EVENT_LOOP_DELAY_S);
    }

    exit(EXIT_SUCCESS);
#else
    DEBUG_LOG_INFO("[SIM] ModelSimulation_Init, Won't be initialized.");
#endif
}

void ModelSimulation_Destroy(void)
{

}

void ModelSimulation_SendMessage(model_simulation_message_type_t message_type, float * data, int data_len)
{
        if (!SystemUtility_SendMessage(simulation_fifo_name, (int)message_type, data, data_len))
    {
        DEBUG_LOG_ERROR("[SIM] ModelSimulation_SendMessage, Can't send message to gui process!");
    }
}
