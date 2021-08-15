#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

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

/*** MODEL SIMULATION INTERATION ***/

sig_atomic_t simulation_iteration;

/*** MODEL SIMULATION TIMER ***/

system_timer_t simulation_timer;
#define GetSimulationTimer()    (&simulation_timer)

/*** MODEL_SIMULATION_INTERVAL_STEP ***/

#define MODEL_SIMULATION_STEP_INTERVAL_MS       (5)

/*** MODEL SIMULATION MESSAGE FIFO ***/

#define MAX_SIMULATION_FLOAT_DATA_LEN           (10)

const char *simulation_fifo_name = "simulation_fifo";

pthread_mutex_t mutex_force = PTHREAD_MUTEX_INITIALIZER;
#define GetForceMutex()   (&mutex_force)

#define DEFAULT_VALUE (0)

/*** INIT STATES ***/

#define INIT_STATE_X1           0
#define INIT_STATE_X1_DOT       0
#define INIT_STATE_Y1           5
#define INIT_STATE_Y1_DOT       0
#define INIT_STATE_Y2           0

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
#define HILL_STEP               (0.01)

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
            // modelSimluation_GenerateRamp();
            DEBUG_LOG_ERROR("SIM] modelSimluation_GenerateRoad, road not implemented!");
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

/*** STATE SPACE MODEL - INITIAL STATES ***/

#define INITIAL_STATES_ROW_SIZE 5
#define INITIAL_STATES_COLUMN_SIZE 1

static float INITIAL_STATES_matrix[INITIAL_STATES_ROW_SIZE][INITIAL_STATES_COLUMN_SIZE];

static Mat INITIAL_STATES = {(float *)INITIAL_STATES_matrix, INITIAL_STATES_ROW_SIZE, INITIAL_STATES_COLUMN_SIZE};

#define GetINITIAL_STATES() (&INITIAL_STATES)

static void modelSimluation_InitMatrixINITIAL_STATES(void)
{
    INITIAL_STATES_matrix[0][0] = INIT_STATE_X1;
    INITIAL_STATES_matrix[1][0] = INIT_STATE_X1_DOT;
    INITIAL_STATES_matrix[2][0] = INIT_STATE_Y1 ;
    INITIAL_STATES_matrix[3][0] = INIT_STATE_Y1_DOT;
    INITIAL_STATES_matrix[4][0] = INIT_STATE_Y2;
}

/*** STATE SPACE MODEL - PREVIOUS STATES ***/

#define XK_1_STATES_ROW_SIZE INITIAL_STATES_ROW_SIZE
#define XK_1_STATES_COLUMN_SIZE INITIAL_STATES_COLUMN_SIZE

static float XK_1_matrix[XK_1_STATES_ROW_SIZE][XK_1_STATES_COLUMN_SIZE];

static Mat XK_1 = {(float *)XK_1_matrix, XK_1_STATES_ROW_SIZE, XK_1_STATES_COLUMN_SIZE};

#define GetXK_1_STATES() (&XK_1)

static void modelSimluation_InitMatrixXK_1(void)
{
    XK_1_matrix[0][0] = 0;
    XK_1_matrix[1][0] = 0;
    XK_1_matrix[2][0] = 0 ;
    XK_1_matrix[3][0] = 0;
    XK_1_matrix[4][0] = 0;
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

        /* send fifo messages */
        Gui_SendMessage(gui_message_model_simulation_data, model_simulation_data, MODEL_SIMULATION_DATA_SIZE);
        Control_SendMessage(control_message_model_states, model_simulation_data, MODEL_SIMULATION_DATA_SIZE);

        /* send mqueue messages */
        sensor_mqueue_message_t sensor_message;
        sensor_message.state_X1 = get(x, 1, 1);
        sensor_message.state_X1_dot = get(x, 2, 1);
        sensor_message.state_Y1 = get(x, 3, 1);
        sensor_message.state_Y1_dot = get(x, 4, 1);
        Sensor_SendMQueueMessage(&sensor_message, sizeof(sensor_message));
    }
    else
    {
        DEBUG_LOG_ERROR("[SIM] modelSimluation_SendModelStates, wrong matrix!!!");
    }
}

static void modelSimluation_SimulionStart(void)
{
    /* set simulation iteration variable to 0 */
    simulation_iteration = 0;

    /* Init road input */
    modelSimluation_GenerateRoad(road_type_hills);

    /* init simulation matrixes */
    modelSimluation_InitMatrixA();
    modelSimluation_InitMatrixB();
    modelSimluation_InitMatrixC();
    modelSimluation_InitMatrixINITIAL_STATES();

    /* init x(k-1) states matrix */
    modelSimluation_InitMatrixXK_1();
}

static void modelSimluation_SimulationEnd(void)
{
    /* send states to control and gui process (Xd and Yd) */
    modelSimluation_SendModelStates(GetXK_1_STATES(), road[SIM_TIME-1], (int)SIM_TIME-1);

    /* set simulation iteration variable to 0 */
    simulation_iteration = 0;
}

static void *modelSimluation_SimulationStepThread(void *cookie)
{
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self(), thread_priority_max);

    if (simulation_iteration < 0 || simulation_iteration >= simulation_time)
    {
        SystemUtility_DestroyCyclicThread(GetSimulationTimer());
        return 0;
    }

    if (simulation_iteration < simulation_time)
    {
        if (simulation_iteration == 0)
        {
            pthread_mutex_lock(GetForceMutex());

            // set input matrix
            Mat *INPUT = newmat(INPUT_ROW_SIZE, INPUT_COLUMN_SIZE, DEFAULT_VALUE);
            set(INPUT, 1, 1, force);
            set(INPUT, 2, 1, road[simulation_iteration]); // this will be update by mesage from control process

            pthread_mutex_unlock(GetForceMutex());

            // send states to control and gui process (Xd and Yd)
            modelSimluation_SendModelStates(GetINITIAL_STATES(), road[simulation_iteration], simulation_iteration);

            // calculate x
            Mat *a_x = multiply(GetA(), GetINITIAL_STATES());
            Mat *b_i = multiply(GetB(), INPUT);

            Mat *xk = sum(a_x, b_i);
            set(GetXK_1_STATES(), 1, 1, get(xk, 1, 1));
            set(GetXK_1_STATES(), 2, 1, get(xk, 2, 1));
            set(GetXK_1_STATES(), 3, 1, get(xk, 3, 1));
            set(GetXK_1_STATES(), 4, 1, get(xk, 4, 1));
            set(GetXK_1_STATES(), 5, 1, get(xk, 5, 1));

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
            set(INPUT, 2, 1, road[simulation_iteration]); // this will be update by mesage from control process

            pthread_mutex_unlock(GetForceMutex());

            // send states to control and gui process (Xd and Yd)
            modelSimluation_SendModelStates(GetXK_1_STATES(), road[simulation_iteration], simulation_iteration);

            // calculate x
            Mat *a_x = multiply(GetA(), GetXK_1_STATES());
            Mat *b_i = multiply(GetB(), INPUT);

            Mat *xk = sum(a_x, b_i);
            set(GetXK_1_STATES(), 1, 1, get(xk, 1, 1));
            set(GetXK_1_STATES(), 2, 1, get(xk, 2, 1));
            set(GetXK_1_STATES(), 3, 1, get(xk, 3, 1));
            set(GetXK_1_STATES(), 4, 1, get(xk, 4, 1));
            set(GetXK_1_STATES(), 5, 1, get(xk, 5, 1));

            freemat(INPUT);
            freemat(a_x);
            freemat(b_i);
            freemat(xk);
        }

        simulation_iteration++;
    }

    return 0;
}

static void *modelSimluation_ReceiveMessageThread(void *cookie)
{
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self(), thread_priority_high);

    /* create message fifo queue */
    if (!SystemUtility_CreateMessageFifo(simulation_fifo_name))
    {
        DEBUG_LOG_ERROR("[SIM] modelSimluation_ReceiveMessageThread, Can't create FIFO!");
        return 0;
    }

    gui_message_type_t message_type = gui_message_unknown;
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

                    if (float_data_len == 1)
                    {
                        #ifdef INCLUDE_CONTROLER

                            #ifdef FEEDFORWARD_CONTRLER
                                force += float_data[0]; // last control force value add to last force value
                            #endif /* FEEDFORWARD_CONTRLER */

                            #ifdef PID_CONTROLLER
                                force = float_data[0];
                            #endif /* PID_CONTROLLER */

                            // DEBUG_LOG_DEBUG("[SIM] FORCE: %f", force);

                        #endif /* INCLUDE_CONTROLER */
                    }

                    pthread_mutex_unlock(GetForceMutex());
                break;
                default:
                break;
            }
        }
    }

    return 0;
}

/*** GLOBAL FUNCTION ***/

void ModelSimulation_Init(void)
{
#ifdef INIT_MODEL_SIMULATION

    DELAY_S(1);

    /* Init receive message */
    if (!SystemUtility_CreateThread(modelSimluation_ReceiveMessageThread))
    {
        DEBUG_LOG_ERROR("[SIM] ModelSimulation_Init, Can't create receive thread!");
    }

    /* START SIMULATION */
    modelSimluation_SimulionStart();

    /* Perform simulation steps */
    if (!SystemUtility_CreateCyclicThread(modelSimluation_SimulationStepThread, GetSimulationTimer(), MODEL_SIMULATION_STEP_INTERVAL_MS))
    {
        DEBUG_LOG_ERROR("[SIM] ModelSimulation_Init, Can't create simulaton cyclic step thread!");
    }

    /* STOP SIMULATION */
    modelSimluation_SimulationEnd();

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
    remove(simulation_fifo_name);
}

void ModelSimulation_SendMessage(model_simulation_message_type_t message_type, float * data, int data_len)
{
    if (!SystemUtility_SendMessage(simulation_fifo_name, (int)message_type, data, data_len))
    {
        DEBUG_LOG_ERROR("[SIM] ModelSimulation_SendMessage, Can't send message to gui process!");
    }
}
