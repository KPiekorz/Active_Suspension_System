#include "auto_model.h"
#include <stdio.h>
#include "accelerometer_manager.h"

void AutoModel_ModelState(void)
{
    printf("Model state!!!\n");
    printf("This is extra model!!!\n");

    AccelerometerManager_Init();
}