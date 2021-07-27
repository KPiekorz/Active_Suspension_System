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

/*** VARIABLES ***/

const double m1 = 2500;
const double m2 = 320;
const double k1 = 80000;
const double k2 = 500000;
const double b1 = 350;
const double b2 = 15020;

/*** STATE SPACE MODEL - A ***/

#define A_ROW_SIZE      4
#define A_COLUMN_SIZE   4

static double A_matrix[A_ROW_SIZE][A_COLUMN_SIZE];

static Mat A = {(double *)A_matrix, A_ROW_SIZE, A_ROW_SIZE};

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

static double B_matrix[B_ROW_SIZE][B_COLUMN_SIZE];

static Mat B = {(double *)B_matrix, B_ROW_SIZE, B_COLUMN_SIZE};

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

static double C_matrix[C_ROW_SIZE][C_COLUMN_SIZE];

static Mat C = {(double *)C_matrix, C_ROW_SIZE, C_COLUMN_SIZE};

#define GetC() (&C)

static void modelSimluation_InitMatrixC(void)
{
    C_matrix[0][0] = 0;
    C_matrix[0][1] = 0;
    C_matrix[0][2] = 1;
    C_matrix[0][3] = 0;
}

/*** STATIC FUNCTION ***/

static void * modelSimluation_SimulationStepThread(void *cookie)
{
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self());

    modelSimluation_InitMatrixA();
    modelSimluation_InitMatrixB();
    modelSimluation_InitMatrixC();

    showmat(GetA());
    showmat(GetB());
    showmat(GetC());

    while (true)
    {
        DEBUG_LOG_VERBOSE("[GUI] modelSimluation_SimulationStepThread");

        DELAY_S(2);
    }

    return 0;
}

/*** GLOBAL FUNCTION ***/

void ModelSimulation_Init(void)
{
    #ifdef INIT_MODEL_SIMULATION
        DEBUG_LOG_DEBUG("ModelSimulation_Init, Init model simulation process...");

        /* Init simulation of suspension */
        if (!SystemUtility_CreateThread(modelSimluation_SimulationStepThread))
        {
            DEBUG_LOG_ERROR("[GUI] ModelSimulation_Init, Can't create simulaton step thread!");
        }

        while (1)
        {
            float data[4] = {3.02, 1.1, 1.23, 5.01};

            // Gui_SendMessage(gui_message_control_signal, data, 4);

            DELAY_S(2);

            DEBUG_LOG_VERBOSE("[SIM] Model simulation process running...");
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

}
