#include <stdio.h>
#include <math.h>
#include <stdlib.h>

float inclined_plane(float x, float y, float x0, float y0, float azim, float dip);
float Two_D_Gaussian(float x, float y, float x0, float y0, float A, float sigma);

int main(int argc, char *argv[])
{
  /* Nx:    number of Nodes in X direction  */
  /* Ny:    number of Nodes in Y direction  */
  /* dx:    node spacing in X */
  /* dy:    node spacing in Y */
  /* fx:    node (0,0) X Coordinate */
  /* fy:    node (0,0) Y Coordinate */

  /* zref:  Horizon reference depth */

  int    option; /* 0=inclined_plane, 1=Two_D_Gaussian */
  int    Nx, Ny;
  float  fx, fy;
  float  dx, dy;
  float  zref;
  float  azim, dip;
  float  A, sigma;
  int    i, j;
  float  x, y, z, x0, y0;
  FILE  *F;
  char  *end;

  fprintf(stderr,"Program Horizon2D - horizon maker\n");
  fprintf(stderr,"Usage: \n");

  fprintf(stderr,
"horizon2D option Nx Ny dx dy fx fy zref x0 y0 azim dip >horiz_out.txt\n");
  fprintf(stderr,"or\n");
  fprintf(stderr,
"horizon2D option Nx Ny dx dy fx fy zref x0 y0 A sigma >horiz_out.txt\n\n");

  option = atoi(argv[1]);
  Nx     = atoi(argv[2]);
  Ny     = atoi(argv[3]);
  dx     = atof(argv[4]);
  dy     = atof(argv[5]);
  fx     = atof(argv[6]);
  fy     = atof(argv[7]);
  fprintf(stderr,"(Nx, Ny, dx, dy, fx, fy): (%d, %d, %.1f,  %.1f,  %.1f, %.1f)\n",
           Nx, Ny, dx, dy, fx, fy);


  zref   = atof(argv[8]);
  x0     = atof(argv[9]);
  y0     = atof(argv[10]);

  if (option == 0) {
    azim   = atof(argv[11]);
    dip    = atof(argv[12]);
    fprintf(stderr,"Inclined plane parameters:\n");
    fprintf(stderr,"(zref, x0, y0, azim, dip): (%.1f, %.1f, %.1f,  %.1f,  %.1f)\n",
             zref, x0, y0, azim, dip);
  }
  else {
    A      = atof(argv[11]);
    sigma  = atof(argv[12]);
    fprintf(stderr,"Two-D Gaussian function parameters:\n");
    fprintf(stderr,"(zref, x0, y0, A, sigma): (%.1f, %.1f, %.1f,  %.1f,  %.1f)\n",
             zref, x0, y0, A, sigma);
  }

  //F = fopen("horizon1.txt","w");

  for (i=0; i<Nx; i++) for (j=0; j<Ny; j++) {
    x = fx + i * dx;
    y = fy + j * dy;
    if (option == 0) {
      z = zref + inclined_plane(x, y, x0, y0, azim, dip);
    }
    else {
      z = zref + Two_D_Gaussian(x, y, x0, y0, A, sigma);
    }
    fprintf(stdout,"%f\n",z);
  }

  //fclose(F);
  return 0;
}

float Two_D_Gaussian(float x, float y, float x0, float y0, float A, float sigma)
{
  float XX, YY, SS;

  XX = (x0 - x)*(x0 - x);
  YY = (y0 - y)*(y0 - y);
  SS = sigma*sigma;

  return  A * exp( -1 * ( XX/(2*SS)  + YY/(2*SS) ));
}


float inclined_plane(float x, float y, float x0, float y0, float azim, float dip)
{
  float PI=3.1416;
  float alpha, n1x, n1y, m, a, b, c, dist;

  if (dip > 89.9) dip= 89.9;
  if (dip < 0) dip=0;

  dip = (PI/180.0)*dip;  // dip at 90 degrees from azim

  alpha = (PI/180.0)*(90 - azim);

  n1x = cos(alpha);
  n1y = sin(alpha);

  if (n1x != 0) {
    m = n1y/n1x;
  }
  else m = 99999999.9;

  a = m;
  b = -1;
  c = y0 - m*x0;

  dist = (a*x + b*y + c)/sqrt(a*a + b*b);

  return dist * tan(dip);
}

