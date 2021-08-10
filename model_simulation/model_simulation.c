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
    road_type_zero,
    road_type_step,
    road_type_random,
    road_type_impuls,
    road_type_ramp,
    road_type_hills,

} road_type_t;

/*** INCLUDE CONTROLER ***/

#define INCLUDE_CONTROLER

/*** MODEL_SIMULATION_INTERVAL_STEP ***/

#define MODEL_SIMULATION_STEP_INTERVAL_MS       (5)

/*** MODEL SIMULATION MESSAGE FIFO ***/

#define MAX_SIMULATION_FLOAT_DATA_LEN           (5)

const char *simulation_fifo_name = "simulation_fifo";

pthread_mutex_t mutex_force = PTHREAD_MUTEX_INITIALIZER;
#define GetForceMutex()   (&mutex_force)

#define DEFAULT_VALUE (0)

/*** SIMULATION PARAMETERS ***/

const float simulation_time = SIM_TIME; // how many iteration will be performed
const float sampling_period = SAMPLE_TIME;

#define STEP_VALUE  10
#define ST_TIME     40
const float step_time = ST_TIME;

#define IMPULSE_VALUE   10
#define IM_TIME         20
const float impulse_time = IM_TIME;

/**
 * @note Road and force will be set in input matrix in every interation
 */

/*** INPUT CONTROL SIGNAL - FORCE ***/

#define INITIAL_FORCE_VALUE     (0)
static float force = INITIAL_FORCE_VALUE; // this varialbe will be update from control process (access to force variable have to be done through mutex semaphore)

/*** INPUT ROAD SIGNAL ***/

static float road[SIM_TIME]; // for now it will be only step signal, in 10 step in will be 10 [cm] (0.1 [m])

static void modelSimluation_GenerateZero(void)
{
    for (int i = 0; i < simulation_time; i++)
    {
        road[i] = 0;
    }
}

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

#define NUMBER_OF_HILLS         (2)
#define HILL_STEP               (1)

static void  modelSimluation_GenerateHills(void)
{
    if (simulation_time < 1000)
    {
        DEBUG_LOG_ERROR("[SIM] modelSimluation_GenerateHills, invalid simulation  time!");
        return;
    }

    int change_inclination = simulation_time / (NUMBER_OF_HILLS * 2);
    int sign = (1);

    for (int i = 1; i < simulation_time; i++)
    {
        if ((i % change_inclination) == 0)
        {
            sign *= (-1);
        }

        road[i] += road[i-1] + (sign * HILL_STEP);
    }
}

static void modelSimluation_GenerateRoad(road_type_t type)
{
    switch (type)
    {
        case road_type_zero:
            modelSimluation_GenerateZero();
        break;
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
        case road_type_hills:
            modelSimluation_GenerateHills();
        break;
        default:
        DEBUG_LOG_WARN("[SIM] modelSimluation_GenerateRoad, unknown road!");
        break;
    }
}

/*** VARIABLES ***/

const float m1 = 250;
const float m2 = 32;
const float k1 = 80;
const float k2 = 500;
const float b1 = 35;
const float b2 = 150;

/*** STATE SPACE MODEL - A ***/

#define A_ROW_SIZE 5
#define A_COLUMN_SIZE 5

static float A_matrix[A_ROW_SIZE][A_COLUMN_SIZE];

static Mat A = {(float *)A_matrix, A_ROW_SIZE, A_ROW_SIZE};

#define GetA() (&A)

static void modelSimluation_InitMatrixA(void)
{
    A_matrix[0][0] = 0.8628;
    A_matrix[0][1] = 0.0958;
    A_matrix[0][2] = -0.0044;
    A_matrix[0][3] = -0.0013;
    A_matrix[0][4] = 0;

    A_matrix[1][0] = -2.8741;
    A_matrix[1][1] = 0.8628;
    A_matrix[1][2] = 0.1998;
    A_matrix[1][3] = -0.0178;
    A_matrix[1][4] = 0;

    A_matrix[2][0] = 0.7829;
    A_matrix[2][1] = 0.0864;
    A_matrix[2][2] = -0.0648;
    A_matrix[2][3] = -0.0021;
    A_matrix[2][4] = 0;

    A_matrix[3][0] = -9.7229;
    A_matrix[3][1] = 0.4498;
    A_matrix[3][2] = 1.9570;
    A_matrix[3][3] = -0.1183;
    A_matrix[3][4] = 0;

    A_matrix[4][0] = 0.0864;
    A_matrix[4][1] = 0.0039;
    A_matrix[4][2] = -0.0016;
    A_matrix[4][3] = 0.0006;
    A_matrix[4][4] = 1.0000;
}

/*** STATE SPACE MODEL - B ***/

#define B_ROW_SIZE 5
#define B_COLUMN_SIZE 2

static float B_matrix[B_ROW_SIZE][B_COLUMN_SIZE];

static Mat B = {(float *)B_matrix, B_ROW_SIZE, B_COLUMN_SIZE};

#define GetB() (&B)

static void modelSimluation_InitMatrixB(void)
{
    B_matrix[0][0] = 0;
    B_matrix[0][1] = 0.1372;

    B_matrix[1][0] = 0;
    B_matrix[1][1] = 2.8741;

    B_matrix[2][0] = 0;
    B_matrix[2][1] = -0.7829;

    B_matrix[3][0] = 0.0001;
    B_matrix[3][1] = 9.7229;

    B_matrix[4][0] = 0;
    B_matrix[4][1] = -0.0864;
}

/*** STATE SPACE MODEL - C ***/

#define C_ROW_SIZE 1
#define C_COLUMN_SIZE 5

static float C_matrix[C_ROW_SIZE][C_COLUMN_SIZE];

static Mat C = {(float *)C_matrix, C_ROW_SIZE, C_COLUMN_SIZE};

#define GetC() (&C)

static void modelSimluation_InitMatrixC(void)
{
    C_matrix[0][0] = 0;
    C_matrix[0][1] = 0;
    C_matrix[0][2] = 1;
    C_matrix[0][3] = 0;
    C_matrix[0][4] = 0;
}

/*** STATE SPACE MODEL - STATES ***/

#define INITIAL_STATES_ROW_SIZE 5
#define INITIAL_STATES_COLUMN_SIZE 1

static float INITIAL_STATES_matrix[INITIAL_STATES_ROW_SIZE][INITIAL_STATES_COLUMN_SIZE];

static Mat INITIAL_STATES = {(float *)INITIAL_STATES_matrix, INITIAL_STATES_ROW_SIZE, INITIAL_STATES_COLUMN_SIZE};

#define GetINITIAL_STATES() (&INITIAL_STATES)

static void modelSimluation_InitMatrixINITIAL_STATES(void)
{
    INITIAL_STATES_matrix[0][0] = 0;
    INITIAL_STATES_matrix[1][0] = 0;
    INITIAL_STATES_matrix[2][0] = 0;
    INITIAL_STATES_matrix[3][0] = 0;
    INITIAL_STATES_matrix[4][0] = 0;
}

/*** INPUT MATRIX ***/

#define INPUT_ROW_SIZE 2
#define INPUT_COLUMN_SIZE 1

/*** STATIC FUNCTION ***/

static void modelSimluation_SendModelStates(Mat *x, float road, int iteration)
{
    if (x->row == INITIAL_STATES_ROW_SIZE && x->col == INITIAL_STATES_COLUMN_SIZE)
    {
        pthread_mutex_lock(GetForceMutex());

        float model_simulation_data[MODEL_SIMULATION_DATA_SIZE];
        model_simulation_data[MODEL_SIMULATION_STATE_X1] = get(x, 1, 1);
        model_simulation_data[MODEL_SIMULATION_STATE_X1_DOT] = get(x, 2, 1);
        model_simulation_data[MODEL_SIMULATION_STATE_Y1] = get(x, 3, 1);
        model_simulation_data[MODEL_SIMULATION_STATE_Y1_DOT] = get(x, 4, 1);
        model_simulation_data[MODEL_SIMULATION_STATE_Y2] = get(x, 5, 1);
        model_simulation_data[MODEL_SIMULATION_STATE_ROAD] = road;
        model_simulation_data[MODEL_SIMULATION_STATE_FORCE] = force;
        model_simulation_data[MODEL_SIMULATION_STATE_ITERATION] = (float)iteration;

        pthread_mutex_unlock(GetForceMutex());

        Gui_SendMessage(gui_message_model_simulation_data, model_simulation_data, MODEL_SIMULATION_DATA_SIZE);
        Control_SendMessage(control_message_model_states, model_simulation_data, MODEL_SIMULATION_DATA_SIZE);
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
    modelSimluation_InitMatrixINITIAL_STATES();

    // showmat(GetA());
    // showmat(GetB());
    // showmat(GetC());

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
            set(INPUT, 1, 1, force);
            set(INPUT, 2, 1, road[i]); // this will be update by mesage from control process

            pthread_mutex_unlock(GetForceMutex());

            // send states to control and gui process (Xd and Yd)
            modelSimluation_SendModelStates(GetINITIAL_STATES(), road[i], i);

            // calculate x
            Mat *a_x = multiply(GetA(), GetINITIAL_STATES());
            Mat *b_i = multiply(GetB(), INPUT);

            Mat *xk = sum(a_x, b_i);
            set(xk_1, 1, 1, get(xk, 1, 1));
            set(xk_1, 2, 1, get(xk, 2, 1));
            set(xk_1, 3, 1, get(xk, 3, 1));
            set(xk_1, 4, 1, get(xk, 4, 1));
            set(xk_1, 5, 1, get(xk, 5, 1));

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
            set(INPUT, 1, 1, force);
            set(INPUT, 2, 1, road[i]); // this will be update by mesage from control process

            showmat(INPUT);

            pthread_mutex_unlock(GetForceMutex());

            // send states to control and gui process (Xd and Yd)
            modelSimluation_SendModelStates(xk_1, road[i], i);

            // calculate x
            Mat *a_x = multiply(GetA(), xk_1);
            Mat *b_i = multiply(GetB(), INPUT);

            Mat *xk = sum(a_x, b_i);
            set(xk_1, 1, 1, get(xk, 1, 1));
            set(xk_1, 2, 1, get(xk, 2, 1));
            set(xk_1, 3, 1, get(xk, 3, 1));
            set(xk_1, 4, 1, get(xk, 4, 1));
            set(xk_1, 5, 1, get(xk, 5, 1));

            freemat(INPUT);
            freemat(a_x);
            freemat(b_i);
            freemat(xk);
        }

        DELAY_MS(MODEL_SIMULATION_STEP_INTERVAL_MS);
    }

    // send states to control and gui process (Xd and Yd)
    modelSimluation_SendModelStates(xk_1, road[SIM_TIME-1], (int)SIM_TIME-1);

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

                            #ifdef FEEDFORWARD_CONTRLER
                                force += float_data[0]; // last control force value add to last force value
                            #endif /* FEEDFORWARD_CONTRLER */

                            #ifdef PID_CONTROLLER
                                force += float_data[0];
                            #endif /* PID_CONTROLLER */

                        #else
                            // force = 0; // last control force value add to last force value
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
    modelSimluation_GenerateRoad(road_type_hills);

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
