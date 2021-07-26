#include "control.h"
#include "system_utility.h"

void Control_Init(void)
{
    #ifdef INIT_CONTROL
        DEBUG_LOG_DEBUG("Control_Init, Init model simulation process...");

        while (1)
        {
            DEBUG_LOG_VERBOSE("Control_Init, Control process running...");
		    DELAY_S(2);
        }

        exit(EXIT_SUCCESS);
    #else
        DEBUG_LOG_INFO("[SIM] Control_Init, Won't be initialized.");
    #endif
}

void Control_Destroy(void)
{
    DEBUG_LOG_DEBUG("Control_Destroy, Destroy CONTROL process...");

}