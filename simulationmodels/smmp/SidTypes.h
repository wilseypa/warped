//-*-c++-*-
#ifndef SIDTYPES_HH
#define SIDTYPES_HH

#include <stddef.h>

typedef long long Time;

enum distribution_t {UNIFORM, POISSON, EXPONENTIAL, NORMAL, BINOMIAL, FIXED,
	       ALTERNATE, ROUNDROBIN, CONDITIONAL, ALL, SPECIAL};
#endif
