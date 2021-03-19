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
		printf("Car simulation...\n");
		fflush(stdout);

		Delay(2);
	}

	printf("Type 'q' to quit both processes.\n");
	while(getc(stdin) == 'q') {}
}

