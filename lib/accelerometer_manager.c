#include "accelerometer_manager.h"
#include "stdio.h"

void AccelerometerManager_Init(void)
{
    printf("Init accelerometer manager module...");

	printf("Type 'q' to quit both processes.\n");
	while(getc(stdin) == 'q') {}
}