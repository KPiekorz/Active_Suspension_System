#include <stdio.h>
// #include <conio.h>
#include "accelerometer_manager.h"
#include "auto_model.h"

int main(int argc, char *argv[])
{
    AutoModel_ModelState();

    int i;
    if( argc >= 2 )
    {
        printf("The arguments supplied are:\n");
        for(i = 1; i < argc; i++)
        {
            printf("%s\n", argv[i]);
        }
    }
    else
    {
        printf("argument list is empty.\n");
    }
    return 0;
}