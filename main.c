#ifndef _POSIX_C_SOURCE
	#define _POSIX_C_SOURCE 200112L /* Or higher */
#endif

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "model_simulation.h"
#include "system_utility.h"
#include "system_process.h"

#include "gui.h"

/* static helper function prototype */
static void main_PrintArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    main_PrintArgs(argc, argv);

    int f_arr_size = 2;
    float f_arr[f_arr_size];
    f_arr[0] = 1.32;
    f_arr[1] = 3.45;

    const int b_arr_size = GET_FLOAT_DATA_SIZE(f_arr_size);
    byte b_arr[b_arr_size];
    memset(b_arr, 0, b_arr_size);

    DEBUG_LOG_DEBUG("Float i: %f", f_arr[0]);

    int b_arr_used_size = SystemUtility_SetFloatArrayInByteArray(f_arr, f_arr_size, b_arr, b_arr_size);

    if (b_arr_used_size > 0)
    {
        DEBUG_LOG_DEBUG("Size: %d", b_arr_used_size);
    }
    else
    {
        DEBUG_LOG_DEBUG("Error!");
    }

    memset(f_arr, 0, sizeof(float) * f_arr_size);

    int f_arr_used_size = SystemUtility_GetFloatArrayFromByteArray(b_arr, b_arr_used_size, f_arr, f_arr_size);
    if (f_arr_used_size > 0)
    {
        DEBUG_LOG_DEBUG("Size: %d", f_arr_used_size);
    }
    else
    {
        DEBUG_LOG_DEBUG("Error!");
    }

    // SystemProcess_Initialize();

    DEBUG_LOG_ALWAYS("Enter q to quit: ");
  	while(getc(stdin)!='q') {}

    // SystemProcess_Destroy();

	return EXIT_SUCCESS;
}

/**
 * @brief  Print program arguments entered by user.
 * @note
 * @param  argc:
 * @param  *argv[]:
 * @retval None
 */
static void main_PrintArgs(int argc, char *argv[])
{
    int i;
    if( argc >= 2 )
    {
        DEBUG_LOG_DEBUG("The arguments supplied are:");
        for(i = 1; i < argc; i++)
        {
            DEBUG_LOG_DEBUG("%s", argv[i]);
        }
    }
    else
    {
        DEBUG_LOG_DEBUG("Argument list is empty!");
    }
}