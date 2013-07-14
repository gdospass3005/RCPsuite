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
  float  fpeak;
  char   fileTwtPP[255];
  char   fileTwtPS[255];
  char   fileRPP[255];
  char   fileRPS[255];
  char   fileSynthPP[255];
  char   fileSynthPS[255];
} inputpar;
/******************************************************************************/

void add_ricker(float *Synth, int nt, float dt, float fpeak, 
                float R, float Twt, int nt_half);


int main(int argc, char *argv[])
{
  inputpar  q;

  float  *TwtPP;
  float  *TwtPS;
  float  *RPP;
  float  *RPS;
  float  *SynthPP;
  float  *SynthPS;

  FILE  *trc_TwtPP;
  FILE  *trc_TwtPS;
  FILE  *trc_RPP;
  FILE  *trc_RPS;
  FILE  *trc_SynthPP;
  FILE  *trc_SynthPS;

  float  offset, h;
  float  z;
  float  t;

  int    ix, iy;
  int    iz, it;
  int    io;

  int    nt_half;

  /* Input information */

  /* # Control parameters */
  q.nx           = atoi(argv[1]); /* nx=ny=1 if well-log */
  q.ny           = atoi(argv[2]);
  q.nz           = atoi(argv[3]);
  q.dz           = atof(argv[4]);

  q.noffset      = atoi(argv[5]);
  q.doffset      = atof(argv[6]);
  q.offset0      = atof(argv[7]);

  q.nt           = atoi(argv[8]);
  q.dt           = atof(argv[9]);
  q.fpeak        = atof(argv[10]);

  /* # Input data */
  strcpy( q.fileTwtPP,   argv[11]);
  strcpy( q.fileTwtPS,   argv[12]);
  strcpy( q.fileRPP,     argv[13]);
  strcpy( q.fileRPS,     argv[14]);
  strcpy( q.fileSynthPP, argv[15]);
  strcpy( q.fileSynthPS, argv[16]);

  nt_half = 1 + (( 1.0 / q.fpeak ) / q.dt);

  trc_TwtPP    = fopen( q.fileTwtPP,   "rb");
  trc_TwtPS    = fopen( q.fileTwtPS,   "rb");
  trc_RPP      = fopen( q.fileRPP,     "rb");
  trc_RPS      = fopen( q.fileRPS,     "rb");
  trc_SynthPP  = fopen( q.fileSynthPP, "wb");
  trc_SynthPS  = fopen( q.fileSynthPS, "wb");

  TwtPP   = alloc1float(q.nz);
  TwtPS   = alloc1float(q.nz);
  RPP     = alloc1float(q.nz);
  RPS     = alloc1float(q.nz);
  SynthPP = alloc1float(q.nt);
  SynthPS = alloc1float(q.nt);

  /* # Loop over traces */
  for (ix=0;ix<q.nx;ix++) for (iy=0;iy<q.ny;iy++) {

    fread(TwtPP,   sizeof(float), (size_t) q.nz, trc_TwtPP);
    fread(TwtPS,   sizeof(float), (size_t) q.nz, trc_TwtPS);
    fread(RPP,     sizeof(float), (size_t) q.nz, trc_RPP);
    fread(RPS,     sizeof(float), (size_t) q.nz, trc_RPS);

    for (it=0; it<q.nt; ++it) { SynthPP[it] = 0.0; SynthPS[it] = 0.0; }

    for (iz=0; iz<q.nz; ++iz) {
      add_ricker(SynthPP, q.nt, q.dt, q.fpeak, RPP[iz], TwtPP[iz], nt_half);
    }

    fwrite(SynthPP, sizeof(float), (size_t) q.nt, trc_SynthPP);
  }

  fclose(trc_TwtPP);
  fclose(trc_TwtPS);
  fclose(trc_RPP);
  fclose(trc_RPS);
  fclose(trc_SynthPP);
  fclose(trc_SynthPS);
}



void add_ricker(float *Synth, int nt, float dt, float fpeak, 
                float R, float Twt, int nt_half) 
{

  int    it, itmin, itmax;
  int    it_r;  // closest sample from PP reflectivity TWT time
  float  t, tk;

  it_r = floor( Twt / dt );

  itmin = it_r - nt_half;
  itmax = it_r + nt_half;

  if (itmin < 0)  itmin = 0;
  if (itmax > nt) itmax = nt;

  for (it=itmin; it<itmax; ++it) {
    t = it * dt;

    tk = Twt - t;

    Synth[it] += R * exp(-PI*PI*fpeak*fpeak*tk*tk)*
                       (1.0-2.*PI*PI*fpeak*fpeak*tk*tk);
  }
}


