#include "rcps.h"

/******************************************************************************/
typedef struct INPUTPARMS
{ 
  int    nx;
  int    ny;
  int    nt;
  int    na;
  float  da;
  float  a0;
  char   fileVP[255];
  char   fileVS[255];
  char   fileRO[255];
  char   fileRPP[255];
  char   fileRPS[255];
} inputpar;
/******************************************************************************/

int main(int argc, char *argv[])
{
  inputpar  q;
  layerpro  p;
  interfpro r;
  float  prev_Vp, prev_Vs, prev_Ro;
  float  Vp, Vs, Ro;
  float  theta, phi, Rpp, Rps;

  FILE  *trc_RPP;
  FILE  *trc_RPS;
  FILE  *trc_VP;
  FILE  *trc_VS;
  FILE  *trc_RO;

  int    ix, iy;
  int    ia;
  int    it, itr, ntr;
  float  buff;

  /* Input information */
  q.nx           = atoi(argv[1]);
  q.ny           = atoi(argv[2]);
  q.nt           = atoi(argv[3]);
  q.na           = atoi(argv[4]);
  q.da           = atof(argv[5]);
  q.a0           = atof(argv[6]);
  strcpy( q.fileVP,  argv[7]);
  strcpy( q.fileVS,  argv[8]);
  strcpy( q.fileRO,  argv[9]);
  strcpy( q.fileRPP, argv[10]);
  strcpy( q.fileRPS, argv[11]);

  /* AVA Reflectivity Modeling */
  trc_RPP     = fopen( q.fileRPP, "wb");
  trc_RPS     = fopen( q.fileRPS, "wb");

  for (ia=0;ia<q.na;ia++) {
    theta = (q.a0 + ia*q.da);
    theta = theta*(PI / 180.0);

    trc_VP = fopen( q.fileVP, "rb");
    trc_VS = fopen( q.fileVS, "rb");
    trc_RO = fopen( q.fileRO, "rb");

    for (ix=0;ix<q.nx;ix++) for (iy=0;iy<q.ny;iy++) {

      fread(&buff, sizeof(float), (size_t) 1, trc_VP);
      prev_Vp = buff;
      fread(&buff, sizeof(float), (size_t) 1, trc_VS);
      prev_Vs = buff;
      fread(&buff, sizeof(float), (size_t) 1, trc_RO);
      prev_Ro = buff;

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
      }
    }

    fclose(trc_VP);
    fclose(trc_VS);
    fclose(trc_RO);
  }

  fclose(trc_RPP);
  fclose(trc_RPS);
}



