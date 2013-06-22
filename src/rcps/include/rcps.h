/******************************************************************************/
/* rcps.h - include file for general purpose RCPsuite stuff */

#ifndef RCPS_H
#define RCPS_H

/******************************************************************************/
/* INCLUDES */
#include <cwp.h>  /* this include has all relevant C library includes */


/******************************************************************************/
/* TYPEDEFS */
typedef struct LAYERPROPERTY
{ /* Layer Properties */
  float    Vp_upper;
  float    Vs_upper;
  float    Ro_upper;
  float    Vp_lower;
  float    Vs_lower;
  float    Ro_lower;
} layerpro;

typedef struct INTERFPROPERTY
{ /* Interface Properties */
  float    VP;
  float    VS;
  float    RO;
  float    dVp;
  float    dVs;
  float    dRo;
  float    gamma;
} interfpro;


/******************************************************************************/
/* FUNCTION PROTOTYPES */
#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif

/* interface.c */
int    f_layers_to_interface(layerpro  p, interfpro *r);

float  f_Rpp(interfpro r, float theta);
float  f_Rps(interfpro r, float theta, float *phi);

/* errormsg.c */
void   f_error(int flag);

#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

/******************************************************************************/
#endif /* RCPS_H */
