#include "testing.h"
#include <stdio.h>

void pruebas_abb_alumno(void);

/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/

int main(void)
{
    printf("~~~ PRUEBAS ALUMNO ~~~\n");
    pruebas_abb_alumno();

    return failure_count() > 0;
}
