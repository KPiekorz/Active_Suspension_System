#include "gyro_manager.h"
#include "stdio.h"

void GyroManager_Init(void)
{
    printf("Init gyroscope manager module...");

	printf("Type 'q' to quit both processes.\n");
	while(getc(stdin) == 'q') {}
}