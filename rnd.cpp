/*
 * RANDOM.C -- Implements Park & Miller's "Minimum Standard" RNG
 *
 * (Reference:  CACM, Oct 1988, pp 1192-1201)
 *
 * NextRand:  Computes next random integer
 * UnifInt:   Yields an long uniformly distributed between given bounds
 * UnifReal: ields a real uniformly distributed between given bounds
 * Exponential: Yields a real exponentially distributed with given mean
 *
 */

#include "config.h"
#include <stdio.h>
#include <math.h>
#if (defined(LINUX) || defined(_POSIX_SOURCE))
#include <stdint.h>
#endif
#ifdef IBM
#include <inttypes.h>
#endif
#ifdef SUN
#include <inttypes.h>
#endif
#ifdef ATT
#include <sys/bitypes.h>
#endif
#ifdef WIN32
#define int32_t __int32
#endif
#include "dss.h"
#include "rnd.h"

char *env_config PROTO((char *tag, char *dflt));
void NthElement(int64_t, int64_t *);

void dss_random(int64_t *tgt, int64_t lower, int64_t upper, long stream) {
  *tgt = UnifInt(lower, upper, stream);
  Seed[stream].usage += 1;

  return;
}

void row_start(int t) {
  int i;
  for (i = 0; i <= MAX_STREAM; i++)
    Seed[i].usage = 0;

  return;
}

void row_stop(int t) {
  int i;

  /* need to allow for handling the master and detail together */
  if (t == ORDER_LINE)
    t = ORDER;
  if (t == PART_PSUPP)
    t = PART;

  for (i = 0; i <= MAX_STREAM; i++)
    if ((Seed[i].table == t) || (Seed[i].table == tdefs[t].child)) {
      if (set_seeds && (Seed[i].usage > Seed[i].boundary)) {
        fprintf(stderr, "\nSEED CHANGE: seed[%d].usage = %lld\n", i,
                Seed[i].usage);
        Seed[i].boundary = Seed[i].usage;
      } else {
        NthElement((Seed[i].boundary - Seed[i].usage), &Seed[i].value);
#ifdef RNG_TEST
        Seed[i].nCalls += Seed[i].boundary - Seed[i].usage;
#endif
      }
    }
  return;
}

void dump_seeds(int tbl) {
  int i;

  for (i = 0; i <= MAX_STREAM; i++)
    if (Seed[i].table == tbl)
#ifdef RNG_TEST
      printf("%d(%lld):\t%lld\n", i, Seed[i].nCalls, Seed[i].value);
#else
      printf("%d:\t%lld\n", i, Seed[i].value);
#endif
  return;
}

/******************************************************************

   NextRand:  Computes next random integer

*******************************************************************/

/*
 * long NextRand( long nSeed )
 */
int64_t NextRand(int64_t nSeed)

/*
 * nSeed is the previous random number; the returned value is the
 * next random number. The routine generates all numbers in the
 * range 1 .. nM-1.
 */

{
  nSeed = (nSeed * 16807) % 2147483647;
  return (nSeed);
}

/******************************************************************

   UnifInt:  Yields an long uniformly distributed between given bounds

*******************************************************************/

/*
 * long UnifInt( long nLow, long nHigh, long nStream )
 */
int64_t UnifInt(int64_t nLow, int64_t nHigh, long nStream)

/*
 * Returns an integer uniformly distributed between nLow and nHigh,
 * including * the endpoints.  nStream is the random number stream.
 * Stream 0 is used if nStream is not in the range 0..MAX_STREAM.
 */

{
  double dRange;
  int64_t nTemp, nRange;
  int32_t nLow32 = (int32_t)nLow, nHigh32 = (int32_t)nHigh;

  if (nStream < 0 || nStream > MAX_STREAM)
    nStream = 0;

  if ((nHigh == MAX_LONG) && (nLow == 0)) {
    dRange = DOUBLE_CAST(nHigh32 - nLow32 + 1);
    nRange = nHigh32 - nLow32 + 1;
  } else {
    dRange = DOUBLE_CAST(nHigh - nLow + 1);
    nRange = nHigh - nLow + 1;
  }

  Seed[nStream].value = NextRand(Seed[nStream].value);
#ifdef RNG_TEST
  Seed[nStream].nCalls += 1;
#endif
  nTemp = (int64_t)(((double)Seed[nStream].value / dM) * (dRange));
  return (nLow + nTemp);
}
