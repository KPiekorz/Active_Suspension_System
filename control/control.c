#include "control.h"
#include "system_utility.h"

void Control_Init(void)
{
    DEBUG_LOG_DEBUG("Init CONTROL process...");

    while (1)
    {
        DEBUG_LOG_VERBOSE("Control process running...");
		DELAY_S(2);
    }
}

void Control_Destroy(void)
{
    DEBUG_LOG_DEBUG("Destroy CONTROL process...");

}