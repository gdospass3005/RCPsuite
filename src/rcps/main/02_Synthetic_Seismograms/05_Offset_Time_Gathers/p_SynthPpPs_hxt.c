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
  char   fileThetaPP[255];
  char   fileThetaPS[255];
  char   fileTwtPP[255];
  char   fileTwtPS[255];
  char   fileRPP[255];
  char   fileRPS[255];
  char   fileSynthPP_t[255];
  char   fileSynthPS_t[255];
  char   fileNmoPP_t[255];
  char   fileNmoPS_t[255];
  char   fileThetaPP_t[255];
  char   fileThetaPS_t[255];
} inputpar;
/******************************************************************************/

void add_ricker(float *Synth, int nt, float dt, float fpeak, 
                float R, float Twt, int nt_half);
void ztot_Nmo(float *Twt, float *Twt_zero, int nz, int nt, float dt, 
                float *Nmo);
void ztot_Angle(float *Twt, float *Theta, int nz, int nt, float dt, 
                float *Theta_t);


int main(int argc, char *argv[])
{
  inputpar  q;

  float *ThetaPP;
  float *ThetaPS;
  float *TwtPP;
  float *TwtPS;
  float *RPP;
  float *RPS;
  float *SynthPP_t;
  float *SynthPS_t;
  float *NmoPP_t;
  float *NmoPS_t;
  float *ThetaPP_t;
  float *ThetaPS_t;

  FILE  *trc_ThetaPP;
  FILE  *trc_ThetaPS;
  FILE  *trc_TwtPP;
  FILE  *trc_TwtPS;
  FILE  *trc_RPP;
  FILE  *trc_RPS;
  FILE  *trc_SynthPP_t;
  FILE  *trc_SynthPS_t;
  FILE  *trc_NmoPP_t;
  FILE  *trc_NmoPS_t;
  FILE  *trc_ThetaPP_t;
  FILE  *trc_ThetaPS_t;

  float  *TwtPP_zero;
  float  *TwtPS_zero;
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
  strcpy( q.fileThetaPP,   argv[11]);
  strcpy( q.fileThetaPS,   argv[12]);
  strcpy( q.fileTwtPP,     argv[13]);
  strcpy( q.fileTwtPS,     argv[14]);
  strcpy( q.fileRPP,       argv[15]);
  strcpy( q.fileRPS,       argv[16]);
  strcpy( q.fileSynthPP_t, argv[17]);
  strcpy( q.fileSynthPS_t, argv[18]);
  strcpy( q.fileNmoPP_t,   argv[19]);
  strcpy( q.fileNmoPS_t,   argv[20]);
  strcpy( q.fileThetaPP_t, argv[21]);
  strcpy( q.fileThetaPS_t, argv[22]);

  nt_half = 1 + (( 1.0 / q.fpeak ) / q.dt);

  trc_ThetaPP    = fopen( q.fileThetaPP,   "rb");
  trc_ThetaPS    = fopen( q.fileThetaPS,   "rb");
  trc_TwtPP      = fopen( q.fileTwtPP,     "rb");
  trc_TwtPS      = fopen( q.fileTwtPS,     "rb");
  trc_RPP        = fopen( q.fileRPP,       "rb");
  trc_RPS        = fopen( q.fileRPS,       "rb");
  trc_SynthPP_t  = fopen( q.fileSynthPP_t, "wb");
  trc_SynthPS_t  = fopen( q.fileSynthPS_t, "wb");
  trc_NmoPP_t    = fopen( q.fileNmoPP_t,   "wb");
  trc_NmoPS_t    = fopen( q.fileNmoPS_t,   "wb");
  trc_ThetaPP_t  = fopen( q.fileThetaPP_t, "wb");
  trc_ThetaPS_t  = fopen( q.fileThetaPS_t, "wb");

  ThetaPP        = alloc1float(q.nz);
  ThetaPS        = alloc1float(q.nz);
  TwtPP          = alloc1float(q.nz);
  TwtPS          = alloc1float(q.nz);
  TwtPP_zero     = alloc1float(q.nz);
  TwtPS_zero     = alloc1float(q.nz);
  RPP            = alloc1float(q.nz);
  RPS            = alloc1float(q.nz);
  SynthPP_t      = alloc1float(q.nt);
  SynthPS_t      = alloc1float(q.nt);
  NmoPP_t        = alloc1float(q.nt);
  NmoPS_t        = alloc1float(q.nt);
  ThetaPP_t      = alloc1float(q.nt);
  ThetaPS_t      = alloc1float(q.nt);

  /* # Loop over traces */
  for (ix=0;ix<q.nx;ix++) for (iy=0;iy<q.ny;iy++) {

    /* # Loop over offsets */
    for (io=0;io<q.noffset;io++) {

      fread(TwtPP,   sizeof(float), (size_t) q.nz, trc_TwtPP);
      fread(TwtPS,   sizeof(float), (size_t) q.nz, trc_TwtPS);
      fread(RPP,     sizeof(float), (size_t) q.nz, trc_RPP);
      fread(RPS,     sizeof(float), (size_t) q.nz, trc_RPS);
      fread(ThetaPP, sizeof(float), (size_t) q.nz, trc_ThetaPP);
      fread(ThetaPS, sizeof(float), (size_t) q.nz, trc_ThetaPS);

      if (io==0) {
        for (iz=0; iz<q.nz; ++iz) {
          TwtPP_zero[iz] = TwtPP[iz];
          TwtPS_zero[iz] = TwtPS[iz];
        }
      }

      for (it=0; it<q.nt; ++it) { SynthPP_t[it] = 0.0; SynthPS_t[it] = 0.0; }

      for (iz=0; iz<q.nz; ++iz) {
        add_ricker(SynthPP_t, q.nt, q.dt, q.fpeak, RPP[iz], TwtPP[iz], nt_half);
        add_ricker(SynthPS_t, q.nt, q.dt, q.fpeak, RPS[iz], TwtPS[iz], nt_half);
      }

      ztot_Nmo( TwtPP, TwtPP_zero, q.nz, q.nt, q.dt, NmoPP_t );
      ztot_Nmo( TwtPS, TwtPS_zero, q.nz, q.nt, q.dt, NmoPS_t );
      ztot_Angle( TwtPP, ThetaPP, q.nz, q.nt, q.dt, ThetaPP_t );
      ztot_Angle( TwtPS, ThetaPS, q.nz, q.nt, q.dt, ThetaPS_t );

      fwrite(ThetaPP_t, sizeof(float), (size_t) q.nt, trc_ThetaPP_t);
      fwrite(ThetaPS_t, sizeof(float), (size_t) q.nt, trc_ThetaPS_t);
      fwrite(SynthPP_t, sizeof(float), (size_t) q.nt, trc_SynthPP_t);
      fwrite(SynthPS_t, sizeof(float), (size_t) q.nt, trc_SynthPS_t);
      fwrite(NmoPP_t,   sizeof(float), (size_t) q.nt, trc_NmoPP_t);
      fwrite(NmoPS_t,   sizeof(float), (size_t) q.nt, trc_NmoPS_t);
    }
  }

  fclose(trc_TwtPP);
  fclose(trc_TwtPS);
  fclose(trc_RPP);
  fclose(trc_RPS);
  fclose(trc_ThetaPP);
  fclose(trc_ThetaPS);
  fclose(trc_SynthPP_t);
  fclose(trc_SynthPS_t);
  fclose(trc_NmoPP_t);
  fclose(trc_NmoPS_t);
  fclose(trc_ThetaPP_t);
  fclose(trc_ThetaPS_t);
}


void ztot_Angle(float *Twt, float *Theta, int nz, int nt, float dt, 
              float *Theta_t)
{
  float *yin;
  float *xout;
  int    iz, it;

  xout = alloc1float(nt);

  for (it=0; it<nt; ++it) xout[it] = it * dt;

  intlin ( nz, Twt, Theta, Theta[0], Theta[nz-1], nt, xout, Theta_t );

  free1float(xout);
}

void ztot_Nmo(float *Twt, float *Twt_zero, int nz, int nt, float dt, 
              float *Nmo)
{
  float *yin;
  float *xout;
  int    iz, it;

  yin  = alloc1float(nz);
  xout = alloc1float(nt);

  for (iz=0; iz<nz; ++iz) yin[iz] = Twt[iz] - Twt_zero[iz];
  for (it=0; it<nt; ++it) xout[it] = it * dt;

  intlin ( nz, Twt, yin, yin[0], yin[nz-1], nt, xout, Nmo );

  free1float(yin);
  free1float(xout);
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


