#include "sensors.h"
#include <stdio.h>

#include "gui.h"
#include "system_utility.h"


void Sensor_Init(void)
{

    while (1)
    {
        DEBUG_LOG_VERBOSE("Sensor process running...");
		DELAY_S(2);
    }
}

void Sensor_Destroy(void)
{
    DEBUG_LOG_INFO("Init CONTROL process...");
}
