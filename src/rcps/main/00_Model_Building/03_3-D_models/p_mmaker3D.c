#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <cwp.h>
#define MAXNHOR 100

/******************************************************************************/
typedef struct INPUTPARMS
{ 
  int      Nx, Ny, Nz;
  float    fz, dz;
  int      nHor;
  char     file_hList[255];
  char     file_mList[255];
  char     file_out[255];
} inputpar;                                                                                                      
/******************************************************************************/

int main(int argc, char *argv[])
{
  inputpar  q;
  /* Nx:    number of Nodes in X direction     */
  /* Ny:    number of Nodes in Y direction     */
  /* Nz:    number of Nodes in Z direction     */
  /* fz:    node (0,0,0) Z Coordinate (depth)  */
  /* dz:    node spacing in Z                  */
  /* NHor:  number of horizons                 */
  /* nLayers (= NHor+1 ) --> number of layers  */

  int      nLayers;
  int     *HorType;
  float ***Hor;
  float   *trc_out;
  float    z;
  char     filename[MAXNHOR][255];
  float    mValue[MAXNHOR+1];
  FILE    *F;
  FILE    *G;
  FILE    *H;
  FILE    *J;
  int      i, ix, iy, iz;
  float    outValue;

  q.Nx     = atoi(argv[1]);
  q.Ny     = atoi(argv[2]);
  q.Nz     = atoi(argv[3]);
  q.fz     = atof(argv[4]);
  q.dz     = atof(argv[5]);
  q.nHor   = atoi(argv[6]);
  strcpy( q.file_hList, argv[7]);
  strcpy( q.file_mList, argv[8]);
  strcpy( q.file_out,   argv[9]);

  if (q.nHor > MAXNHOR) {
   fprintf(stderr,"p_mmaker: Too many horizons.\nStop.\n\n");
   return (-1);
  }

  Hor     = alloc3float(q.Ny, q.Nx, q.nHor);
  HorType = alloc1int(q.nHor);

  nLayers = q.nHor+1;

  F = fopen( q.file_hList ,"r");
  G = fopen( q.file_mList ,"r");

  fscanf(G,"%f",&mValue[0]);
  fprintf(stderr,"First layer value: %f\n",mValue[0]);

  /* Input Parametres and Horizons */
  for (i=0;i<q.nHor;++i) {
   fscanf(F,"%s",filename[i]);
   fscanf(F,"%d",&HorType[i]);
   fscanf(G,"%f",&mValue[i+1]);
   fprintf(stderr,"%s (Type: %d) --> Value below: %f\n",
     filename[i],HorType[i],mValue[i+1]);

   H = fopen(filename[i],"r");
   for (ix=0;ix<q.Nx;++ix) for (iy=0;iy<q.Ny;++iy) {
     fscanf(H,"%f",&Hor[i][ix][iy]);
   }
   fclose(H);
  }
  fclose(G);
  fclose(F);

  /* Reshape horizons on crossings */
  for (ix=0;ix<q.Nx;++ix) for (iy=0;iy<q.Ny;++iy) {
   for (i=1;i<q.nHor;++i) {
    if ((Hor[i][ix][iy] < Hor[i-1][ix][iy]) && (!HorType[i]))
      Hor[i][ix][iy] = Hor[i-1][ix][iy];
   }
  }

  /* Output Model Traces */
  J = fopen( q.file_out ,"wb");
  trc_out = alloc1float(q.Nz);
  for (ix=0;ix<q.Nx;++ix) for (iy=0;iy<q.Ny;++iy) {

   for (iz=0;iz<q.Nz;++iz) {
    z = q.fz + iz * q.dz;
    trc_out[iz] = mValue[0];
    for (i=0;i<q.nHor;++i) {
     if (z > Hor[i][ix][iy]) trc_out[iz] = mValue[i+1];
    }
   }

   fwrite(trc_out, sizeof(float), (size_t) q.Nz, J);
  }
  fclose(J);

  return (0);
}
