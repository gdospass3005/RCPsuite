#include "rcps.h"


/******************************************************************************/
void f_error(int flag)
{
  switch (flag) {
    case 1:
     fprintf(stderr,"\nERROR: Input error.\n"); exit(0);
    case 2:
     fprintf(stderr,"\nERROR: Calculations error.\n"); exit(0);
    default:
     exit(0);
   }
}


