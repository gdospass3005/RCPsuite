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
  char   fileVP[255];
  char   fileVS[255];
  char   fileRO[255];
  char   fileTwtPP[255];
  char   fileTwtPS[255];
  char   fileThetaPP[255];
  char   fileThetaPS[255];
  char   filePhi[255];
  char   fileRPP[255];
  char   fileRPS[255];
} inputpar;
/******************************************************************************/

int main(int argc, char *argv[])
{
  inputpar  q;
  layerpro  p;
  interfpro r;

  float  *VP;
  float  *VS;
  float  *RO;

  float  *TwtPP;
  float  *TwtPS;
  float  *ThetaPP;
  float  *ThetaPS;
  float  *Phi;
  float  *RPP;
  float  *RPS;

  float  offset, h;
  float  theta, phi, Rpp, Rps;

  FILE  *trc_VP;
  FILE  *trc_VS;
  FILE  *trc_RO;

  FILE  *trc_TwtPP;
  FILE  *trc_TwtPS;
  FILE  *trc_ThetaPP;
  FILE  *trc_ThetaPS;
  FILE  *trc_Phi;
  FILE  *trc_RPP;
  FILE  *trc_RPP;

  float  *Vavg;
  float  *Delta_S;
  float  *Delta_t;

  int    ix, iy;
  int    iz;
  int    io;

  /* Input information */

  /* # Control parameters */
  q.nx           = atoi(argv[1]);
  q.ny           = atoi(argv[2]);
  q.nz           = atoi(argv[3]);
  q.dz           = atof(argv[4]);
  q.noffset      = atoi(argv[5]);
  q.doffset      = atof(argv[6]);
  q.offset0      = atof(argv[7]);

  /* # Input data (logs or model in z) */
  strcpy( q.fileVP,     argv[8]);
  strcpy( q.fileVS,     argv[9]);
  strcpy( q.fileRO,     argv[10]);

  /* # Output data */
  strcpy( q.fileTwtPP,   argv[11]);
  strcpy( q.fileTwtPS,   argv[12]);
  strcpy( q.fileThetaPP, argv[13]);
  strcpy( q.fileThetaPS, argv[14]);
  strcpy( q.filePhi,     argv[15]);
  strcpy( q.fileRPP,     argv[16]);
  strcpy( q.fileRPS,     argv[17]);

  /* AVO Reflectivity Modeling */
  /* Obs: 'convolve' later by adding time delayed scaled wavelets to time trace */

  /* # Opening input files */
  trc_VP = fopen( q.fileVP, "rb");
  trc_VS = fopen( q.fileVS, "rb");
  trc_RO = fopen( q.fileRO, "rb");

  /* # Opening output files */
  trc_TwtPP   = fopen( q.fileTwtPP, "wb");
  trc_TwtPS   = fopen( q.fileTwtPS, "wb");
  trc_ThetaPP  = fopen( q.fileThetaPP, "wb");
  trc_ThetaPS  = fopen( q.fileThetaPS, "wb");
  trc_Phi      = fopen( q.filePhi, "wb");
  trc_RPP      = fopen( q.fileRPP, "wb");
  trc_RPS      = fopen( q.fileRPS, "wb");

  /* # Memory allocation for required quantities */
  VP = alloc1float(q.nz);
  VS = alloc1float(q.nz);
  RO = alloc1float(q.nz);

  Vavg    = alloc1float(q.nz);
  Delta_S = alloc1float(q.nz);
  Delta_t = alloc1float(q.nz);

  TwtPP   = alloc1float(q.nz);
  TwtPS   = alloc1float(q.nz);
  ThetaPP = alloc1float(q.nz);
  ThetaPS = alloc1float(q.nz);
  Phi     = alloc1float(q.nz);
  RPP     = alloc1float(q.nz);
  RPS     = alloc1float(q.nz);

  /* # Loop over model traces */
  for (ix=0;ix<q.nx;ix++) for (iy=0;iy<q.ny;iy++) {

    fread(VP, sizeof(float), (size_t) q.nz, trc_VP);
    fread(VS, sizeof(float), (size_t) q.nz, trc_VS);
    fread(RO, sizeof(float), (size_t) q.nz, trc_RO);

    /* # Loop over offsets */
    for (io=0;io<q.noffset;io++) {
      offset = (q.offset0 + io*q.doffset);
      h = offset / 2.0;

      /* 1st Z-sample initialization */
      Vavg[0] = VP[0];

      TwtPP[0]    = (2.0 * h) / Vavg[0];
      TwtPS[0]    = TwtPP[0];
      ThetaPP[0]  = PI/2.0;
      ThetaPS[0]  = PI/2.0;
      Phi[0]      = ThetaPS[0];
      RPP[0]      = 0.0;
      RPS[0]      = 0.0;

      for (iz=1; iz<q.nz; iz++) {
      //theta = tg(h/z);




/*
      buff = 0.0;
      fwrite(&buff, sizeof(float), (size_t) 1, trc_RPP);
      fwrite(&buff, sizeof(float), (size_t) 1, trc_RPS);
      for (it=1;it<q.nt;it++) {

        fread(&buff, sizeof(float), (size_t) 1, trc_VP);
        Vp = buff;
        fread(&buff, sizeof(float), (size_t) 1, trc_VS);
        Vs = buff;
        fread(&buff, sizeof(float), (size_t) 1, trc_RO);
        Ro = buff;

        p.Vp_upper  = prev_Vp;
        p.Vs_upper  = prev_Vs;
        p.Ro_upper  = prev_Ro;

        p.Vp_lower  = Vp;
        p.Vs_lower  = Vs;
        p.Ro_lower  = Ro;

        if ( f_layers_to_interface(p, &r) == 1 ) f_error(1);

        Rpp = f_Rpp(r, theta);
        Rps = f_Rps(r, theta, &phi);

        fwrite(&Rpp, sizeof(float), (size_t) 1, trc_RPP);
        fwrite(&Rps, sizeof(float), (size_t) 1, trc_RPS);

        prev_Vp = Vp;
        prev_Vs = Vs;
        prev_Ro = Ro;
      } */
    }

    fclose(trc_VP);
    fclose(trc_VS);
    fclose(trc_RO);
  }

  fclose(trc_RPP);
  fclose(trc_RPS);
}



