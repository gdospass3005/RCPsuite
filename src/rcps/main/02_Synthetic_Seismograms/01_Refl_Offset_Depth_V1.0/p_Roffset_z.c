/* AVO Reflectivity Modeling (in depth).

   RPP from Fatti's A&R approximation
   RPS from A&R approximation

   nx, ny, nz:  # of samples in X, Y, Z directions
   dz:          sample spacing in Z
   noffset:     # of offsets
   doffset:     offset spacing
   offset0:     first offset
   trc_VP:      input  VP model (1-, 2- or 3-D)
   trc_VS:      input  VS model 
   trc_RO:      input  density model 
   trc_ThetaPP: output PP incidence/reflection angle
   trc_ThetaPS: output PS incidence angle
   trc_Phi:     output PS reflection angle
   trc_TwtPP:   output PP Two-Way Time
   trc_TwtPS:   output PS Two-Way Time
   trc_RPP:     output PP reflectivity
   trc_RPS:     output PS reflectivity
*/

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
  char   fileThetaPP[255];
  char   fileThetaPS[255];
  char   filePhi[255];
  char   fileTwtPP[255];
  char   fileTwtPS[255];
  char   fileRPP[255];
  char   fileRPS[255];
} inputpar;
/******************************************************************************/

void P_Roffset_z_usage(void);


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

  FILE  *trc_VP;
  FILE  *trc_VS;
  FILE  *trc_RO;

  FILE  *trc_TwtPP;
  FILE  *trc_TwtPS;
  FILE  *trc_ThetaPP;
  FILE  *trc_ThetaPS;
  FILE  *trc_Phi;
  FILE  *trc_RPP;
  FILE  *trc_RPS;

  float *VavgP;
  float *VavgS;
  float  offset, h;
  float  z;
  float  theta, phi;
  float  S;
  float  TP, TS;
  float  gamma, xc0, dxc;
  float  xc;

  int    ix, iy;
  int    iz, iiz;
  int    io;

  /* Input information */
  if (argc == 1) P_Roffset_z_usage();

  /* # Control parameters */
  q.nx           = atoi(argv[1]); /* nx=ny=1 if well-log */
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
  strcpy( q.fileThetaPP, argv[11]);
  strcpy( q.fileThetaPS, argv[12]);
  strcpy( q.filePhi,     argv[13]);
  strcpy( q.fileTwtPP,   argv[14]);
  strcpy( q.fileTwtPS,   argv[15]);
  strcpy( q.fileRPP,     argv[16]);
  strcpy( q.fileRPS,     argv[17]);

  /* AVO Reflectivity Modeling (in depth) */
  /* Obs: 'convolve' later by adding time delayed scaled wavelets to time trace */

  /* # Opening input files */
  trc_VP = fopen( q.fileVP, "rb");
  trc_VS = fopen( q.fileVS, "rb");
  trc_RO = fopen( q.fileRO, "rb");

  /* # Opening output files */
  trc_ThetaPP  = fopen( q.fileThetaPP, "wb");
  trc_ThetaPS  = fopen( q.fileThetaPS, "wb");
  trc_Phi      = fopen( q.filePhi,     "wb");
  trc_TwtPP    = fopen( q.fileTwtPP,   "wb");
  trc_TwtPS    = fopen( q.fileTwtPS,   "wb");
  trc_RPP      = fopen( q.fileRPP,     "wb");
  trc_RPS      = fopen( q.fileRPS,     "wb");

  /* # Memory allocation for required quantities */
  VP = alloc1float(q.nz);
  VS = alloc1float(q.nz);
  RO = alloc1float(q.nz);

  VavgP = alloc1float(q.nz);
  VavgS = alloc1float(q.nz);

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

    VavgP[0] = VP[0];
    VavgS[0] = VS[0];

    /* Zero-offset Average VP and VS */
    for (iz=1; iz<q.nz; iz++) {
      z = iz * q.dz;  /* depth */
      TP = 0.0;
      TS = 0.0;
      for (iiz=1; iiz<=iz; iiz++) {
        TP += q.dz / VP[iiz-1];
        TS += q.dz / VS[iiz-1];
      }
      VavgP[iz] = z / TP;
      VavgS[iz] = z / TS;
    }

    /* # Loop over offsets */
    for (io=0;io<q.noffset;io++) {
      offset = (q.offset0 + io*q.doffset);
      h = offset / 2.0;  /* half-offset */

      /* 1st Z-sample */
      ThetaPP[0]  = PI/2.0;
      ThetaPS[0]  = PI/2.0;
      Phi[0]      = 0.0;
      TwtPP[0]    = (2.0 * h) / VP[0];
      TwtPS[0]    = TwtPP[0];
      RPP[0]      = 0.0;
      RPS[0]      = 0.0;

      for (iz=1; iz<q.nz; iz++) {
        z = iz * q.dz;  /* depth */

        /* Interface elastic data preparation */
        p.Vp_upper  = VP[iz-1];
        p.Vs_upper  = VS[iz-1];
        p.Ro_upper  = RO[iz-1];
        p.Vp_lower  = VP[iz];
        p.Vs_lower  = VS[iz];
        p.Ro_lower  = RO[iz];
        if ( f_layers_to_interface(p, &r) == 1 ) f_error(1);

        /* --------------------------------*/
        /* P-P incidence, reflection angle */
        /* --------------------------------*/
        ThetaPP[iz] = atan(h/z);

        /* ------------------------------------*/
        /* P-P two-way time (TWT) calculations */
        /* ------------------------------------*/
        S         = z / cos( ThetaPP[iz] );
        TP        = S / VavgP[iz];
        TwtPP[iz] = 2 * TP;

        /* -----------------*/
        /* P-P reflectivity */
        /* -----------------*/
        RPP[iz] = f_Rpp(r, ThetaPP[iz]);

        /* ---------------------------------------------*/
        /* P-S incidence, reflection angle calculations */
        /* ---------------------------------------------*/

        /* Assimptotic Conversion Point offset (xc0) */
        if (VavgS[iz] < 0.000001) VavgS[iz] = 0.000001;
        gamma = VavgP[iz] / VavgS[iz];
        xc0 = offset * gamma / (1.0 + gamma);

        /* Correction approximation */
        dxc = 0.0;
        if (z < offset) { /* Quadratic corrector for wide angles */
          dxc = ((1.0 - z/offset) * (1.0 - z/offset)) * (offset - xc0);
        }
        xc = xc0 + dxc; /* Conversion point, approximated */

        /* Angles */
        ThetaPS[iz] = atan(xc/z);          /* P-S incidence  angle */
        Phi[iz]     = atan((offset-xc)/z); /* P-S reflection angle */

        /* ------------------------------------*/
        /* P-S two-way time (TWT) calculations */
        /* ------------------------------------*/
        S        = z / cos( ThetaPS[iz] );
        TP       = S / VavgP[iz];
        S        = z / cos( Phi[iz] );
        TS       = S / VavgS[iz];
        TwtPS[iz] = TP + TS;

        /* -----------------*/
        /* P-S reflectivity */
        /* -----------------*/
        RPS[iz] = f_Rps(r, ThetaPP[iz], &phi);
      }

      fwrite(ThetaPP, sizeof(float), (size_t) q.nz, trc_ThetaPP);
      fwrite(ThetaPS, sizeof(float), (size_t) q.nz, trc_ThetaPS);
      fwrite(Phi,     sizeof(float), (size_t) q.nz, trc_Phi);
      fwrite(TwtPP,   sizeof(float), (size_t) q.nz, trc_TwtPP);
      fwrite(TwtPS,   sizeof(float), (size_t) q.nz, trc_TwtPS);
      fwrite(RPP,     sizeof(float), (size_t) q.nz, trc_RPP);
      fwrite(RPS,     sizeof(float), (size_t) q.nz, trc_RPS);
    }
  }

  fclose(trc_ThetaPP);
  fclose(trc_ThetaPS);
  fclose(trc_Phi);
  fclose(trc_TwtPP);
  fclose(trc_TwtPS);
  fclose(trc_RPP);
  fclose(trc_RPS);

  fclose(trc_VP);
  fclose(trc_VS);
  fclose(trc_RO);

}


void P_Roffset_z_usage(void)
{
  fprintf(stderr,
    "p_Roffset_z - AVO Reflectivity Modeling (in depth).\n\n");
  fprintf(stderr,
    "              RPP from Fatti's A&R approximation\n");
  fprintf(stderr,
    "              RPS from A&R approximation\n\n");

  fprintf(stderr,"Usage: \n");
  fprintf(stderr,
    " p_Roffset_z nx ny nz dz noffset doffset offset0 \\ \n");
  fprintf(stderr,
    "        trc_VP trc_VS trc_RO trc_ThetaPP trc_ThetaPS trc_Phi \\ \n");
  fprintf(stderr,
    "        trc_TwtPP trc_TwtPS trc_RPP trc_RPS\n\n");
  fprintf(stderr,
    "Where:\n");
  fprintf(stderr," nx, ny, nz:  # of samples in X, Y, Z directions\n");
  fprintf(stderr," dz:          sample spacing in Z\n");
  fprintf(stderr," noffset:     # of offsets\n");
  fprintf(stderr," doffset:     offset spacing\n");
  fprintf(stderr," offset0:     first offset\n");
  fprintf(stderr," trc_VP:      input  VP model (1-, 2- or 3-D)\n");
  fprintf(stderr," trc_VS:      input  VS model \n");
  fprintf(stderr," trc_RO:      input  density model \n");
  fprintf(stderr," trc_ThetaPP: output PP incidence/reflection angle\n");
  fprintf(stderr," trc_ThetaPS: output PS incidence angle\n");
  fprintf(stderr," trc_Phi:     output PS reflection angle\n");
  fprintf(stderr," trc_TwtPP:   output PP Two-Way Time\n");
  fprintf(stderr," trc_TwtPS:   output PS Two-Way Time\n");
  fprintf(stderr," trc_RPP:     output PP reflectivity\n");
  fprintf(stderr," trc_RPS:     output PS reflectivity\n");
  fprintf(stderr,"\n");

  exit(0);
}

