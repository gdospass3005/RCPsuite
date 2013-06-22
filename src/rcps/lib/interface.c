#include "rcps.h"

/******************************************************************************/
int f_layers_to_interface(layerpro  p, interfpro *r)
{
  r->VP    = 0.5 * (p.Vp_lower + p.Vp_upper);
  r->VS    = 0.5 * (p.Vs_lower + p.Vs_upper);
  r->RO    = 0.5 * (p.Ro_lower + p.Ro_upper);

  if (r->VS < 0.000000001)    r->VS    = 0.000000001;
  r->gamma = r->VP / r->VS ;

  r->dVp   = p.Vp_lower - p.Vp_upper;
  r->dVs   = p.Vs_lower - p.Vs_upper;
  r->dRo   = p.Ro_lower - p.Ro_upper;

  return (0);
}


/******************************************************************************/
float f_Rpp(interfpro r, float theta)
{
  /* Fatti's Rpp(theta) from Romanelli */
  float  A, B, C, k, Rpp;

  k = 4.0 / (r.gamma * r.gamma);

  A = 0.5 * ( (r.dVp / r.VP) + (r.dRo / r.RO) );

  B = A - k * ( (r.dVs / r.VS) + ( ((1.0 + k) / (2.0 * k))
					  * (r.dRo / r.RO) ) );
  C = 0.5 * (r.dVp / r.VP);

  Rpp = A + B * (sin(theta) * sin(theta))
      + C * (sin(theta) * sin(theta)) * (tan(theta) * tan(theta));

  return(Rpp);
}


/******************************************************************************/
float f_Rps(interfpro r, float theta, float *phi)
{
  /* Rps(theta,phi) from Stewart_etal2002 */
  float k, delta, Rps;

  *phi = asin((sin(theta))/r.gamma);

  k = (r.gamma * tan(*phi))/2.0;

  delta = -2 * (sin(theta) * sin(theta)) / (r.gamma * r.gamma)
        + 2 * (cos(theta) * cos(*phi)) / r.gamma;

  Rps = -k * ( (1.0 + delta) * r.dRo / r.RO 
      + 2.0 * delta  * r.dVs / r.VS );

  return(Rps);
}



