/* Normal moveout correction for synthetic PP and PS seismograms and incidence angles

   nx, ny, nt:     # of samples in X, Y and time
   dt:             sample interval in time
   noffset:        # of offsets
   doffset:        offset spacing
   offset0:        first offset
   trc_NmoPP_t:    input PP Nmo correction
   trc_NmoPS_t:    input PS Nmo correction
   trc_SynthPP_t:  input PP synthetic seismogram
   trc_SynthPS_t:  input PS synthetic seismogram
   trc_ThetaPP_t:  input PP incidence angle
   trc_ThetaPS_t:  input PS incidence angle
   trc_SynthPP_to: output PP synthetic seismogram (NMO corrected)
   trc_SynthPS_to: output PS synthetic seismogram (NMO corrected)
   trc_ThetaPP_to: output PP incidence angle (NMO corrected)
   trc_ThetaPS_to: output PS incidence angle (NMO corrected)
*/

#include "rcps.h"

/******************************************************************************/
typedef struct INPUTPARMS
{ 
  int    nx;
  int    ny;
  int    nt;
  float  dt;
  int    noffset;
  float  doffset;
  float  offset0;
  char   fileNmoPP_t[255];
  char   fileNmoPS_t[255];
  char   fileSynthPP_t[255];
  char   fileSynthPS_t[255];
  char   fileThetaPP_t[255];
  char   fileThetaPS_t[255];
  char   fileSynthPP_to[255];
  char   fileSynthPS_to[255];
  char   fileThetaPP_to[255];
  char   fileThetaPS_to[255];
} inputpar;
/******************************************************************************/

void ttoto(float *Nmo, float *A_t, int nt, float dt, float *A_to);
void P_NMO_PpPs_usage(void);

/******************************************************************************/
int main(int argc, char *argv[])
{
  inputpar  q;

  float *NmoPP_t;
  float *NmoPS_t;
  float *SynthPP_t;
  float *SynthPS_t;
  float *ThetaPP_t;
  float *ThetaPS_t;
  float *SynthPP_to;
  float *SynthPS_to;
  float *ThetaPP_to;
  float *ThetaPS_to;

  FILE  *trc_NmoPP_t;
  FILE  *trc_NmoPS_t;
  FILE  *trc_SynthPP_t;
  FILE  *trc_SynthPS_t;
  FILE  *trc_ThetaPP_t;
  FILE  *trc_ThetaPS_t;
  FILE  *trc_SynthPP_to;
  FILE  *trc_SynthPS_to;
  FILE  *trc_ThetaPP_to;
  FILE  *trc_ThetaPS_to;

  float  offset, h;
  float  t;

  int    ix, iy;
  int    it;
  int    io;

  /* Input information */
  if (argc == 1) P_NMO_PpPs_usage();

  /* # Control parameters */
  q.nx           = atoi(argv[1]); /* nx=ny=1 if well-log */
  q.ny           = atoi(argv[2]);
  q.nt           = atoi(argv[3]);
  q.dt           = atof(argv[4]);
  q.noffset      = atoi(argv[5]);
  q.doffset      = atof(argv[6]);
  q.offset0      = atof(argv[7]);

  /* # Input data */
  strcpy( q.fileNmoPP_t,    argv[8]);
  strcpy( q.fileNmoPS_t,    argv[9]);
  strcpy( q.fileSynthPP_t,  argv[10]);
  strcpy( q.fileSynthPS_t,  argv[11]);
  strcpy( q.fileThetaPP_t,  argv[12]);
  strcpy( q.fileThetaPS_t,  argv[13]);
  strcpy( q.fileSynthPP_to, argv[14]);
  strcpy( q.fileSynthPS_to, argv[15]);
  strcpy( q.fileThetaPP_to, argv[16]);
  strcpy( q.fileThetaPS_to, argv[17]);

  trc_NmoPP_t     = fopen( q.fileNmoPP_t,    "rb");
  trc_NmoPS_t     = fopen( q.fileNmoPS_t,    "rb");
  trc_SynthPP_t   = fopen( q.fileSynthPP_t,  "rb");
  trc_SynthPS_t   = fopen( q.fileSynthPS_t,  "rb");
  trc_ThetaPP_t   = fopen( q.fileThetaPP_t,  "rb");
  trc_ThetaPS_t   = fopen( q.fileThetaPS_t,  "rb");
  trc_SynthPP_to  = fopen( q.fileSynthPP_to, "wb");
  trc_SynthPS_to  = fopen( q.fileSynthPS_to, "wb");
  trc_ThetaPP_to  = fopen( q.fileThetaPP_to, "wb");
  trc_ThetaPS_to  = fopen( q.fileThetaPS_to, "wb");

  NmoPP_t         = alloc1float(q.nt);
  NmoPS_t         = alloc1float(q.nt);
  SynthPP_t       = alloc1float(q.nt);
  SynthPS_t       = alloc1float(q.nt);
  ThetaPP_t       = alloc1float(q.nt);
  ThetaPS_t       = alloc1float(q.nt);
  SynthPP_to      = alloc1float(q.nt);
  SynthPS_to      = alloc1float(q.nt);
  ThetaPP_to      = alloc1float(q.nt);
  ThetaPS_to      = alloc1float(q.nt);

  /* # Loop over traces */
  for (ix=0;ix<q.nx;ix++) for (iy=0;iy<q.ny;iy++) {

    /* # Loop over offsets */
    for (io=0;io<q.noffset;io++) {

      fread(NmoPP_t,     sizeof(float), (size_t) q.nt, trc_NmoPP_t);
      fread(NmoPS_t,     sizeof(float), (size_t) q.nt, trc_NmoPS_t);
      fread(SynthPP_t,   sizeof(float), (size_t) q.nt, trc_SynthPP_t);
      fread(SynthPS_t,   sizeof(float), (size_t) q.nt, trc_SynthPS_t);
      fread(ThetaPP_t,   sizeof(float), (size_t) q.nt, trc_ThetaPP_t);
      fread(ThetaPS_t,   sizeof(float), (size_t) q.nt, trc_ThetaPS_t);

      for (it=0; it<q.nt; ++it) { 
        SynthPP_to[it] = 0.0; SynthPS_to[it] = 0.0; 
        ThetaPP_to[it] = 0.0; ThetaPS_to[it] = 0.0; 
      }

      ttoto( NmoPP_t, SynthPP_t, q.nt, q.dt, SynthPP_to );
      ttoto( NmoPS_t, SynthPS_t, q.nt, q.dt, SynthPS_to );
      ttoto( NmoPP_t, ThetaPP_t, q.nt, q.dt, ThetaPP_to );
      ttoto( NmoPS_t, ThetaPS_t, q.nt, q.dt, ThetaPS_to );

      fwrite(SynthPP_to, sizeof(float), (size_t) q.nt, trc_SynthPP_to);
      fwrite(SynthPS_to, sizeof(float), (size_t) q.nt, trc_SynthPS_to);
      fwrite(ThetaPP_to, sizeof(float), (size_t) q.nt, trc_ThetaPP_to);
      fwrite(ThetaPS_to, sizeof(float), (size_t) q.nt, trc_ThetaPS_to);
    }
  }

  fclose(trc_NmoPP_t);
  fclose(trc_NmoPS_t);
  fclose(trc_SynthPP_t);
  fclose(trc_SynthPS_t);
  fclose(trc_ThetaPP_t);
  fclose(trc_ThetaPS_t);
}

void ttoto(float *Nmo, float *A_t, int nt, float dt, float *A_to)
{
  float *to;
  float *xout;
  int    it;

  to    = alloc1float(nt);
  xout  = alloc1float(nt);

  for (it=0; it<nt; ++it) { 
    xout[it] = it * dt;
    to[it]    = xout[it] - Nmo[it];
    if (to[it] < 0) to[it] = 0;
  }

  intlin ( nt, to, A_t, A_t[0], A_t[nt-1], nt, xout, A_to );

  free1float(xout);
  free1float(to);
}


void P_NMO_PpPs_usage(void)
{
  fprintf(stderr,
    "p_NMO_PpPs - Normal moveout correction for synthetic PP and PS seismograms and incidence angles.\n\n");

  fprintf(stderr,"Usage: \n");
  fprintf(stderr,
    " p_NMO_PpPs nx ny nt dt noffset doffset offset0 \\ \n");
  fprintf(stderr,
    "       trc_NmoPP_t trc_NmoPS_t \\ \n");
  fprintf(stderr,
    "       trc_SynthPP_t trc_SynthPS_t \\ \n");
  fprintf(stderr,
    "       trc_ThetaPP_t trc_ThetaPS_t \\ \n");
  fprintf(stderr,
    "       trc_SynthPP_to trc_SynthPS_to \\ \n");
  fprintf(stderr,
    "       trc_ThetaPP_to trc_ThetaPS_to \n");
  fprintf(stderr,
    "Where:\n");
  fprintf(stderr," nx, ny, nt:     # of samples in X, Y and time\n");
  fprintf(stderr," dt:             sample interval in time\n");
  fprintf(stderr," noffset:        # of offsets\n");
  fprintf(stderr," doffset:        offset spacing\n");
  fprintf(stderr," offset0:        first offset\n");
  fprintf(stderr," trc_NmoPP_t:    input PP Nmo correction\n");
  fprintf(stderr," trc_NmoPS_t:    input PS Nmo correction\n");
  fprintf(stderr," trc_SynthPP_t:  input PP synthetic seismogram\n");
  fprintf(stderr," trc_SynthPS_t:  input PS synthetic seismogram\n");
  fprintf(stderr," trc_ThetaPP_t:  input PP incidence angle\n");
  fprintf(stderr," trc_ThetaPS_t:  input PS incidence angle\n");
  fprintf(stderr," trc_SynthPP_to: output PP synthetic seismogram (NMO corrected)\n");
  fprintf(stderr," trc_SynthPS_to: output PS synthetic seismogram (NMO corrected)\n");
  fprintf(stderr," trc_ThetaPP_to: output PP incidence angle (NMO corrected)\n");
  fprintf(stderr," trc_ThetaPS_to: output PS incidence angle (NMO corrected)\n");

  exit(0);
}


