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

    #define DEFAULT_VALUE   0

/*** SIMULATION PARAMETERS ***/

#define MODEL_SIMULATION_DATA_SIZE      6

#define SIM_T                           300

const float simulation_time = SIM_T;
const float sampling_period = 0.5;

/**
 * @note Road and force will be set in input matrix in every interation
 */

static float road[SIM_T]; // for now it will be only step signal, in 10 step in will be 10 [cm] (0.1 [m])

static void modelSimluation_GenerateRoad(void)
{
    // before step
    for (int i = 0; i < 10; i++)
    {
        road[i] = 0;
    }

    // after step
    for (int i = 10; i < simulation_time; i++)
    {
        road[i] = 0.1;
    }
}

static float force = 5; // this varialbe will be update from control process

/*** VARIABLES ***/

const float m1 = 2500;
const float m2 = 320;
const float k1 = 80000;
const float k2 = 500000;
const float b1 = 350;
const float b2 = 15020;

/*** STATE SPACE MODEL - A ***/

#define A_ROW_SIZE      4
#define A_COLUMN_SIZE   4

static float A_matrix[A_ROW_SIZE][A_COLUMN_SIZE];

static Mat A = {(float *)A_matrix, A_ROW_SIZE, A_ROW_SIZE};

#define GetA() (&A)

static void modelSimluation_InitMatrixA(void)
{
    A_matrix[0][0] = 0;
    A_matrix[0][1] = 1;
    A_matrix[0][2] = 0;
    A_matrix[0][3] = 0;

    A_matrix[1][0] = -(b1*b2)/(m1*m2);
    A_matrix[1][1] = 0;
    A_matrix[1][2] = ((b1/m1)*((b1/m1)+(b1/m2)+(b2/m2)))-(k1/m1);
    A_matrix[1][3] = -(b1/m1);

    A_matrix[2][0] = b2/m2;
    A_matrix[2][1] = 0;
    A_matrix[2][2] = -((b1/m1)+(b1/m2)+(b2/m2));
    A_matrix[2][3] = 1;

    A_matrix[3][0] = k2/m2;
    A_matrix[3][1] = 0;
    A_matrix[3][2] = -((k1/m1)+(k1/m2)+(k2/m2));
    A_matrix[3][3] = 0;
}

/*** STATE SPACE MODEL - B ***/

#define B_ROW_SIZE      4
#define B_COLUMN_SIZE   2

static float B_matrix[B_ROW_SIZE][B_COLUMN_SIZE];

static Mat B = {(float *)B_matrix, B_ROW_SIZE, B_COLUMN_SIZE};

#define GetB() (&B)

static void modelSimluation_InitMatrixB(void)
{
    B_matrix[0][0] = 0;
    B_matrix[0][1] = 0;

    B_matrix[1][0] = 1/m1;
    B_matrix[1][1] = (b1*b2)/(m1*m2);

    B_matrix[2][0] = 0;
    B_matrix[2][1] = -(b2/m2);

    B_matrix[3][0] = (1/m1)+(1/m2);
    B_matrix[3][1] = -(k2/m2);
}

/*** STATE SPACE MODEL - C ***/

#define C_ROW_SIZE      1
#define C_COLUMN_SIZE   4

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

#define INITIAL_STATES_ROW_SIZE      4
#define INITIAL_STATES_COLUMN_SIZE   1

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

#define INPUT_ROW_SIZE      2
#define INPUT_COLUMN_SIZE   1

/*** STATIC FUNCTION ***/

static void modelSimluation_SendModelStates(Mat * x, float road)
{
    DEBUG_LOG_DEBUG("modelSimluation_SendModelStates, row: %d, column: %d", x->row, x->col);

    if (x->row == 4 && x->col == 1)
    {
        float model_simulation_data[MODEL_SIMULATION_DATA_SIZE];
        model_simulation_data[0] = get(x, 1, 1);
        model_simulation_data[1] = get(x, 2, 1);
        model_simulation_data[2] = get(x, 3, 1);
        model_simulation_data[3] = get(x, 4, 1);
        model_simulation_data[4] = road;
        model_simulation_data[5] = force;

        Gui_SendMessage(gui_message_model_simulation_data, model_simulation_data, MODEL_SIMULATION_DATA_SIZE);
    }
    else
    {
        DEBUG_LOG_ERROR("modelSimluation_SendModelStates, wrong matrix!!!");
    }
}

static void * modelSimluation_SimulationStepThread(void *cookie)
{
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self());

    modelSimluation_InitMatrixA();
    modelSimluation_InitMatrixB();
    modelSimluation_InitMatrixC();

    // calcualte Ad matrix - discrete matric of A
    Mat * s_a = scalermultiply(GetA(), sampling_period);
    Mat * I = eye(A_ROW_SIZE);
    Mat * before_inv = minus(I, s_a);
    freemat(s_a);
    freemat(I);
    Mat * Ad = inverse(before_inv);
    freemat(before_inv);

    // calcualte Bd matrix - discrete matric of B
    Mat * ad_s = scalermultiply(Ad, sampling_period);
    Mat * Bd = multiply(ad_s, GetB());
    freemat(ad_s);

    // x(k-1)
    Mat * xk_1 = newmat(INITIAL_STATES_ROW_SIZE, INITIAL_STATES_COLUMN_SIZE, DEFAULT_VALUE);

    // actual simulation
    for (int i = 0; i < 1; i++) // simulation_time insted od 1 !!!
    {
        DELAY_MS(10);

        if (i == 0)
        {
            // set input matrix
            Mat * INPUT = newmat(INPUT_ROW_SIZE, INPUT_COLUMN_SIZE, DEFAULT_VALUE);
            set(INPUT, 1, 1, road[i]);
            set(INPUT, 1, 1, force); // this will be update by mesage from control process

            // send states to control and gui process (Xd and Yd)
            modelSimluation_SendModelStates(GetINITIAL_STATES(), road[i]);

            // calculate x
            Mat * a_x = multiply(Ad, GetINITIAL_STATES());
            Mat * b_i = multiply(Bd, INPUT);

            Mat * xk = sum(a_x, b_i);
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
            // set input matrix
            Mat * INPUT = newmat(INPUT_ROW_SIZE, INPUT_COLUMN_SIZE, DEFAULT_VALUE);
            set(INPUT, 1, 1, road[i]);
            set(INPUT, 1, 1, force); // this will be update by mesage from control process

            // send states to control and gui process (Xd and Yd)
            modelSimluation_SendModelStates(xk_1, road[i]);

            // calculate x
            Mat * a_x = multiply(Ad, xk_1);
            Mat * b_i = multiply(Bd, INPUT);

            Mat * xk = sum(a_x, b_i);
            set(xk_1, 1, 1, get(xk, 1, 1));
            set(xk_1, 2, 1, get(xk, 2, 1));
            set(xk_1, 3, 1, get(xk, 3, 1));
            set(xk_1, 4, 1, get(xk, 4, 1));

            freemat(INPUT);
            freemat(a_x);
            freemat(b_i);
            freemat(xk);
        }
    }

    // send states to control and gui process (Xd and Yd)
    modelSimluation_SendModelStates(xk_1, road[SIM_T-1]);

    freemat(Ad);
    freemat(Bd);
    freemat(xk_1);

    return 0;
}

static void * modelSimluation_ReceiveMessageThread(void *cookie)
{
/* init thread with good priority */
    SystemUtility_InitThread(pthread_self());

    // /* create message fifo queue */
    // if (SystemUtility_CreateMessageFifo(gui_fifo_name) == false)
    // {
    //     DEBUG_LOG_ERROR("[GUI] gui_ReceiveMessageThread, Can't create FIFO!");
    //     return 0;
    // }

    // gui_message_type_t message_type = gui_message_unknown;
    // int float_data_len = 20;
    // float float_data[float_data_len];

    // while (true)
    // {
    //     DEBUG_LOG_VERBOSE("[GUI] gui_ReceiveMessageThread, Wait for message!");

    //     /* delay for waiting for another message */
    //     // DELAY_MS(1);

    //     /* try receive message */
    //     if (true == SystemUtility_ReceiveMessage(gui_fifo_name, (int *)&message_type, float_data, &float_data_len))
    //     {
    //         DEBUG_LOG_VERBOSE("[GUI] gui_ReceiveMessageThread, message type: %d, float data len: %d", message_type, float_data_len);

    //         switch (message_type)
    //         {
    //             case gui_message_model_simulation_data:

    //             break;
    //             default:
    //             break;
    //         }

    //         for (int i = 0; i < float_data_len; i++)
    //         {
    //             DEBUG_LOG_VERBOSE("[GUI] gui_ReceiveMessageThread, float[%d]: %f", i, float_data[i]);
    //         }
    //     }
    // }

    return 0;
}

/*** GLOBAL FUNCTION ***/

void ModelSimulation_Init(void)
{
    #ifdef INIT_MODEL_SIMULATION
        DEBUG_LOG_DEBUG("ModelSimulation_Init, Init model simulation process...");

        /* Init road input */
        modelSimluation_GenerateRoad();

        /* Init simulation of suspension */
        if (!SystemUtility_CreateThread(modelSimluation_SimulationStepThread))
        {
            DEBUG_LOG_ERROR("[GUI] ModelSimulation_Init, Can't create simulaton step thread!");
        }

        /* Init simulation of suspension */
        if (!SystemUtility_CreateThread(modelSimluation_ReceiveMessageThread))
        {
            DEBUG_LOG_ERROR("[GUI] ModelSimulation_Init, Can't create receive thread!");
        }

        while (1)
        {
            // float data[4] = {3.02, 1.1, 1.23, 5.01};
            // Gui_SendMessage(gui_message_control_signal, data, 4);

            DELAY_S(10);
        }

        exit(EXIT_SUCCESS);
    #else
        DEBUG_LOG_INFO("[SIM] ModelSimulation_Init, Won't be initialized.");
    #endif
}

void ModelSimulation_Destroy(void)
{
    DEBUG_LOG_DEBUG("Destroy model simulation process...");

}

void ModelSimulation_SendMessage(model_simulation_message_type_t message_type, float * data, int data_len)
{
    DEBUG_LOG_DEBUG("[SIM] ModelSimulation_SendMessage, message type: %d", message_type);

}
