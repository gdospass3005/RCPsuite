#include "rcps.h"

/******************************************************************************/
typedef struct INPUTPARMS
{ /* Calculation Parameters */
  int      option;    /* 1: Rpp; 2: Rps */
  int      na;
  float    da;
  float    a0;
  char     fileRefl[255];
} inputpar;
/******************************************************************************/


int main(int argc, char *argv[])
{
  inputpar  q;
  layerpro  p;
  interfpro r;
  float     Rpp;
  float     Rps;
  float     theta; /* P-wave incidence angle */
  float     phi;   /* S-wave reflection angle */
  float     K_degrees_to_radian = PI / 180.0;
  FILE     *trc_out;
  float     a0, da;
  int       ia;

  fprintf(stderr,"RPP, RPS calculations\n");

  /* Input */
  q.option     = atoi(argv[1]);
  q.na         = atoi(argv[2]);
  q.da         = atof(argv[3]);
  q.a0         = atof(argv[4]);

  p.Vp_upper   = atof(argv[5]);
  p.Vs_upper   = atof(argv[6]);
  p.Ro_upper   = atof(argv[7]);

  p.Vp_lower   = atof(argv[8]);
  p.Vs_lower   = atof(argv[9]);
  p.Ro_lower   = atof(argv[10]);

  strcpy( q.fileRefl, argv[11]);

  /* Properties of interface from properties of layers */
  if ( f_layers_to_interface(p, &r) == 1 ) f_error(1);

  /* Reflectivities calculations */
  trc_out = fopen(q.fileRefl, "w");

  a0 = q.a0 * K_degrees_to_radian;
  da = q.da * K_degrees_to_radian;

  for (ia=0; ia<q.na; ++ia) {
    theta = a0 + ia * da;

    Rpp = f_Rpp(r, theta);

    Rps = f_Rps(r, theta, &phi);

    /* Output */
    fprintf(trc_out, "%.2f %.2f %.5f %.5f\n", 
      theta/ K_degrees_to_radian, phi/ K_degrees_to_radian, Rpp, Rps);
  }

  fprintf(stderr,"Successful end of program.\n");
  return(0);
}



