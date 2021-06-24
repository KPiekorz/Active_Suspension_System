#include "auto_model.h"
#include <stdio.h>
#include "system_utility.h"

/**** STATIC HELPER FUNCTION ****/



/**** GLOBAL FUNCTION ****/

void AutoModel_Init(void)
{
    printf("Init car model module...\n");

	while(1)
	{
		Delay(2);

		printf("Car simulation...\n");
		fflush(stdout);
	}

	printf("Type 'q' to quit both processes.\n");
	while(getc(stdin) == 'q') {}
}

