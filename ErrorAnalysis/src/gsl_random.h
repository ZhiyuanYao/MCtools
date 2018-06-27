#ifndef GSL_RANDOM_H
#define GSL_RANDOM_H
/*------------------------------------------------------------------------------
  random number generator setup in GSL
 ------------------------------------------------------------------------------*/
#include <gsl/gsl_rng.h>
gsl_rng *RNG_ptr;
unsigned long int RNG_seed = 1;
#define rndm() gsl_rng_uniform(RNG_ptr)
#endif
