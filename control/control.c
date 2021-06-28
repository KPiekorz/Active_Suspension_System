#include "control.h"
#include "system_utility.h"

void Control_Init(void)
{
    DEBUG_LOG_DEBUG("Init CONTROL process...");

    while (1)
    {
        DEBUG_LOG_VERBOSE("Control process running...");
		usleep(SEC_TO_US(1));
    }}

void Control_Destroy(void)
{

}