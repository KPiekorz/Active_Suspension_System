#include "auto_model.h"
#include <stdio.h>

/**** STATIC HELPER FUNCTION ****/



/**** GLOBAL FUNCTION ****/

void AutoModel_Init(void)
{
    printf("Init car model task...");

	printf("Type 'q' to quit both processes.\n");
	while(getc(stdin) == 'q') {}
}

