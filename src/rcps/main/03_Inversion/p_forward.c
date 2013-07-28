/* RPP, RPS calculations using approximations to the Zoeppritz equations
   
   RPP from Fatti's A&R approximation
   RPS from A&R approximation

   na: # of angles
   da: angle spacing [degrees]
   a0: first angle [degrees]
   Vp_upper: Vp of the upper medium
   Vs_upper: Vs of the upper medium
   Ro_upper: density of the upper medium
   Vp_lower: Vp of the lower medium
   Vs_lower: Vs of the lower medium
   Ro_lower: density of the lower medium
   fileRefl: output file (theta phi RPP RPS columns)
*/

#include "rcps.h"

/******************************************************************************/
typedef struct INPUTPARMS
{ /* Calculation Parameters */
  int      na;
  float    da;
  float    a0;
  char     fileRefl[255];
} inputpar;
/******************************************************************************/

void Build_D(float **D, int m);
void Multiply_SQMatrix_Vector(float *C, float **A, float *b, int m);

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

  float RP1, RS1, RD1;
  float LP_upper, LP_lower;
  float LS_upper, LS_lower;
  float LD_upper, LD_lower;
  float VP, VS, Gamma;
  float tantheta2, sintheta2;
  float c1, c2, c3, c4, c5;

  float *LP;
  float *LS;
  float *LD;

  float **D;
  float *RP;
  float *RS;
  float *RD;

  fprintf(stderr,
     "p_forward - compare RPP, RPS approximations to the Zoeppritz equations\n");

  if (argc == 1) exit(-1);

  /* Input */
  q.na         = atoi(argv[1]);
  q.da         = atof(argv[2]);
  q.a0         = atof(argv[3]);

  p.Vp_upper   = atof(argv[4]);
  p.Vs_upper   = atof(argv[5]);
  p.Ro_upper   = atof(argv[6]);

  p.Vp_lower   = atof(argv[7]);
  p.Vs_lower   = atof(argv[8]);
  p.Ro_lower   = atof(argv[9]);

  strcpy( q.fileRefl, argv[10]);

  /* RP,RS,RD from LP, LS, LD */
  LP = alloc1float( 2 );
  LS = alloc1float( 2 );
  LD = alloc1float( 2 );

  LP[0] = log( p.Vp_upper * p.Ro_upper );
  LP[1] = log( p.Vp_lower * p.Ro_lower );

  LS[0] = log( p.Vs_upper * p.Ro_upper );
  LS[1] = log( p.Vs_lower * p.Ro_lower );

  LD[0] = log( p.Ro_upper );
  LD[1] = log( p.Ro_lower );

  /* Derivative matrix */
  D  = alloc2float( 2, 2 );
  Build_D(D, 2); /* includes 1/2 factor inside function */

  /* RP = (1/2) * D * LP */
  /* RS = (1/2) * D * LS */
  /* RD = (1/2) * D * LD */
  RP = alloc1float( 2 );
  RS = alloc1float( 2 );
  RD = alloc1float( 2 );

  Multiply_SQMatrix_Vector(RP,D,LP,2);
  Multiply_SQMatrix_Vector(RS,D,LS,2);
  Multiply_SQMatrix_Vector(RD,D,LD,2);

  /* RPP,RPS from LP(VP,RO) LS(VS,RO) LD(RO) and Theta */
  VP = 0.5 * (p.Vp_lower + p.Vp_upper);
  VS = 0.5 * (p.Vs_lower + p.Vs_upper);
  Gamma = VS/VP; /* from Russell et al 2005 */

  a0 = q.a0 * K_degrees_to_radian;
  da = q.da * K_degrees_to_radian;
  trc_out = fopen(q.fileRefl, "w");

  for (ia=0; ia<q.na; ++ia) {
    theta = a0 + ia * da;

    tantheta2 = tan(theta) * tan(theta);
    sintheta2 = sin(theta) * sin(theta);

    c1 =  (1.0) + tantheta2;
    c2 = (-8.0) * Gamma * tantheta2; 
    c3 = (-0.5) * tantheta2 + (2.0) * Gamma * Gamma * sintheta2;
    Rpp = c1 * RP[1] + c2 * RS[1] + c3 * RD[1];

    phi = asin(Gamma * sin(theta));
    c4 = (tan(theta) / Gamma) * ( ((4.0) * sintheta2) 
                                  - ((4.0) * Gamma * cos(theta) * cos(phi))  );
    c5 = (-tan(phi) / ((2.0) * Gamma)) * ( 1.0 + ((2.0) * sin(phi) * sin(phi)) 
                                           - ((2.0) * Gamma * cos(theta) * cos(phi)) );

    Rps = c4 * RS[1] + c5 * RD[1];

    /* Output */
    fprintf(trc_out, "%.2f %.2f %.5f %.5f\n", 
      theta/ K_degrees_to_radian, phi/ K_degrees_to_radian, Rpp, Rps);
  }

  fprintf(stderr,"Successful end of program.\n");
  return(0);
}

void Build_D(float **D, int m)
{
  int i1, i2;

  i1 = 0;
  for (i2=0; i2<m; ++i2)  D[i1][i2] = 0;
 
  for (i1=1; i1<m; ++i1) {
    for (i2=0; i2<m; ++i2)  {
      D[i1][i2] = 0;
      if (i2 == i1)     D[i1][i2] =  1 * 0.5;
      if (i2 == (i1-1)) D[i1][i2] = -1 * 0.5;
    }
  }
}


void Multiply_SQMatrix_Vector(float *C, float **A, float *b, int m)
{
  int i1, i2;

  for (i1=0; i1<m; ++i1) {
    C[i1] = 0;
    for (i2=0; i2<m; ++i2)  C[i1] += A[i1][i2] * b[i2];
  }
}

  /* Properties of interface from properties of layers */
  //if ( f_layers_to_interface(p, &r) == 1 ) f_error(1);

  /* Reflectivities calculations */
  //trc_out = fopen(q.fileRefl, "w");

  //a0 = q.a0 * K_degrees_to_radian;
  //da = q.da * K_degrees_to_radian;

  //for (ia=0; ia<q.na; ++ia) {
  //  theta = a0 + ia * da;

  //  Rpp = f_Rpp(r, theta);

  //  Rps = f_Rps(r, theta, &phi);

    /* Output */
    //fprintf(trc_out, "%.2f %.2f %.5f %.5f\n", 
    //  theta/ K_degrees_to_radian, phi/ K_degrees_to_radian, Rpp, Rps);
  //}


