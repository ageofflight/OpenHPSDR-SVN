#ifndef _correctIQ_h
#define _correctIQ_h

#include <bufvec.h>

typedef struct _iqstate
{
  REAL phase, gain,mu, leakage;
  COMPLEX *w;
  COMPLEX *del;
  COMPLEX *y;
  COMPLEX dc;
  int MASK;
  int index;
} *IQ, iqstate;

extern IQ newCorrectIQ (REAL phase, REAL gain, REAL mu);
extern void delCorrectIQ (IQ iq);
extern void correctIQ (CXB sigbuf, IQ iq, BOOLEAN isTX, int subchan);
#define IQFIRlength 1
#endif
