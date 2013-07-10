#include "rcps.h"

/******************************************************************************/
typedef struct INPUTPARMS
{ 
  int    nx;
  int    ny;
  int    nz;
  float  dz;
  int    noffset;
  float  doffset;
  float  offset0;
  int    nt;
  float  dt;
  char   fileTwtPP[255];
  char   fileTwtPS[255];
  char   fileRPP[255];
  char   fileRPS[255];
} inputpar;
/******************************************************************************/

int main(int argc, char *argv[])
{
  /* 'convolve' by adding time delayed scaled wavelets to time trace */
  inputpar  q;

  float  *TwtPP;
  float  *TwtPS;
  float  *RPP;
  float  *RPS;

  FILE  *trc_TwtPP;
  FILE  *trc_TwtPS;
  FILE  *trc_RPP;
  FILE  *trc_RPS;

  float  offset, h;
  float  z;
  float  t;

  int    ix, iy;
  int    iz, it;
  int    io;

  /* Input information */

  /* # Control parameters */
  q.nx           = atoi(argv[1]); /* nx=ny=1 if well-log */
  q.ny           = atoi(argv[2]);
  q.nz           = atoi(argv[3]);
  q.dz           = atof(argv[4]);

  q.noffset      = atoi(argv[5]);
  q.doffset      = atof(argv[6]);
  q.offset0      = atof(argv[7]);

  /* # Input data */
  strcpy( q.fileTwtPP,   argv[14]);
  strcpy( q.fileTwtPS,   argv[15]);
  strcpy( q.fileRPP,     argv[16]);
  strcpy( q.fileRPS,     argv[17]);

  trc_TwtPP    = fopen( q.fileTwtPP,   "rb");
  trc_TwtPS    = fopen( q.fileTwtPS,   "rb");
  trc_RPP      = fopen( q.fileRPP,     "rb");
  trc_RPS      = fopen( q.fileRPS,     "rb");

  TwtPP   = alloc1float(q.nz);
  TwtPS   = alloc1float(q.nz);
  RPP     = alloc1float(q.nz);
  RPS     = alloc1float(q.nz);

  /* # Loop over model traces */
  for (ix=0;ix<q.nx;ix++) for (iy=0;iy<q.ny;iy++) {

    fread(TwtPP,   sizeof(float), (size_t) q.nz, trc_TwtPP);
    fread(TwtPS,   sizeof(float), (size_t) q.nz, trc_TwtPS);
    fread(RPP,     sizeof(float), (size_t) q.nz, trc_RPP);
    fread(RPS,     sizeof(float), (size_t) q.nz, trc_RPS);



  }

}

