#include <math.h>
#include "gsl_random.h"
/*-----------------------------------------------------------------------------
 * the observables statistics structure, returned by statistics modules
 *----------------------------------------------------------------------------*/
struct TimeStat
{
    double mean;
    double std;    // sigma = TimeStat.std
    double max;
    double min;
    double t_max; // normalized position (time) of max
    double t_min; // normalized position (time) of min
};

/*  structure constructor */
const struct TimeStat TimeStat_Init = {0.0, 0.0, -1.0E100, 1E100, 0.0, 0.0};

/* === Description ======================================================================
 *  Implementation of Nikolay & Boris's method of analysizing the last 3/4 observable
 *  time series data to determine the error bar with one difference.

 *  The difference is that the first fraction of $DUMP_FRACTION data is dumped.
 *  The discard data basically does not help reduce the error bar due to √N law.
 *  However, this helps us get rid of the possible initial non-equilibrium data in cases
 *  where the system is still not thermalized after calling thermalize(). With this setup,
 *  once can even skip the thermalize() part. Therefore, this dumping strategy takes care
 *  of possible issues with thermalization.
 *
 *  When DUMP_FRACTION = 0.0, this code revert to that of Nikolay & Boris.
 *
 *  The array a[] is the averaged observable for statistics in a single block.
 *
 *  DUMP_FRACTION = 1 / (N+1), iBLOCK = (N+1)*M - 1, then index = M, and the
 *  number of blocks that contribute to statistics is N*M. This is why we set N to
 *  be an integer of base 2 power.
 * ====================================================================================*/
#define DUMP_FRACTION (1.0 / (8+1) )
struct TimeStat TimeStatistics(double *datas[], unsigned int iBLOCK)
{
    struct TimeStat stat = TimeStat_Init;
    double *pts[N_VAR], sums[N_VAR], xs[N_VAR];
    unsigned int index, n, i, k;
    index = DUMP_FRACTION * (iBLOCK + 1);
    n = iBLOCK - index + 1;

    /*           |........|...............................|
    a index:     0      index                          iBLOCK
    ptr index:            1     2         ......          n

         ERSTAT is performed for new index in the range [0, 1, ..., n]
         and 0 is discard for counting convenience.
    */

    /*-----------------------------------------------------------------------------
     *  a realization of  ERSTAT(&a[index], n);
     *----------------------------------------------------------------------------*/
    for (k = 0; k < N_VAR; k++) {
        sums[k] = 0.0;
        pts[k] = & datas[k][index];
        for (i = 1; i < n/4; i++ ) sums[k] += *pts[k]++;
        pts[k] = & datas[k][index];
    }

    for (i = n/4; i<=n ; i++)
    {
        for ( k = 0; k < N_VAR; k++) {
            sums[k] += *pts[k]++;
            xs[k] = sums[k] / i;
        }
        stat.mean = func(xs);
        if ( stat.mean > stat.max )
        {
            stat.max = stat.mean;
            stat.t_max = i;
        }
        else if (stat.mean < stat.min)
        {
            stat.min = stat.mean;
            stat.t_min = i;
        }
    }

    stat.std = (stat.max - stat.min) / 2.0;
    stat.t_max = stat.t_max / n;
    stat.t_min = stat.t_min / n;
    return (stat);
}

struct BlockStat
{
    double mean;
    int n;                  // only a[n] contains the std information
    double stds[100];
};

/*  structure constructor */
const struct BlockStat BlockStat_Init = { 0.0, 0.0, {0.0} };

/* const struct BlockStat BlockStat_Init = */

/* ===  FUNCTION  ======================================================================
 *  Description:  a[] = a[iBLOCK+1]
 * ====================================================================================*/
struct BlockStat BlockStatistics(double *datas[], unsigned int iBLOCK)
{
    struct BlockStat stat = BlockStat_Init;
    unsigned int i, j, k, kn, index, n, nn;

    index = DUMP_FRACTION * (iBLOCK + 1);
    nn = iBLOCK - index + 1;

    /*              |........|...............................|
    a index:        0      index                          iBLOCK
    after dump:             0  1  2         ......       nn - 1    -> calculate mean
    b index:                          0,  1,  2,   ...,   n - 1    -> calculate std
    */

    kn = (int) log2(nn + 0.1);   //  2**k <= nn < 2**(k+1)
    n  = (int) pow(2, k);        //  n = 2**k

    printf("block # = %u  statistics blocks # = %u \n", iBLOCK + 1, n);
    if ( n < 64) return stat;

    double sums[N_VAR], sum1, sum2, value, std;
    double *bs[N_VAR], *pts[N_VAR], xs[N_VAR];
    for (k = 0; k < N_VAR; k++) {
        bs[k] = (double *) malloc( n * sizeof(double) );
        pts[k] = & datas[k][iBLOCK - n + 1];
        for (i = 0; i < n; i++ ) bs[k][i] = pts[k][i];
        sums[k] = 0.0;
        for (i = 0; i < n; i++)
            sums[k] += bs[k][i];
        xs[k] = sums[k] / n;
    }

    stat.mean = func(xs);

    // merge blocks to calculate the statistics, at least 64 super blocks
    // two consecutive blocks are merged instead of merging a[i] and a[n/2+i] ,

    for (j = 0; ; j++)
    {
        sum2 = 0.0; sum1 = 0.0;
        for (i = 0; i < n; i++) {
            for (k = 0; k < N_VAR; k++)  xs[k] = bs[k][i];
            value = func(xs);
            sum1 += value;
            sum2 += value * value;
        }
        std = sqrt( 1.0/( n*(n-1) ) * (sum2 - sum1 / n * sum1) );
        stat.stds[j] = std;

        kn = kn - 1;
        n = n / 2;
        if ( n == 32 ) break;

        for (k = 0; k < N_VAR; k++)
            for (i = 0; i < n; i++)
                bs[k][i] = ( bs[k][2*i] + bs[k][2*i+1] ) / 2.0;

    }
    stat.n = j + 1;   // stds[0], stds[1], ..., stds[j]  ==> (j+1) elements

    free(bs);
    return (stat);
}

/* The maximum number of samples = 256 * 2^(BOOTMAX-1) */
#define BOOTMAX 5
struct BootStrapStat
{
    double mean;
    int n;
    double stds[BOOTMAX];
};

const struct BootStrapStat BootStrapStat_Init = {0.0, 0, {0.0} };
/* ===  FUNCTION  ======================================================================
 *  Description: Implements BootStrap method after discarding DUMP_FRACTION data.
 *
 *  BootStrap has the advantage of not relying on BLOCKSIZE in performing error analysis.
 * ====================================================================================*/
struct BootStrapStat BootStrapStatistics(double *datas[], unsigned int iBLOCK)
{
    unsigned int m = 256, times = 0;  // m = BootStrapN
    unsigned int i, j, k, index, n1, n;

    // b[] for BootStrap array, value : the value of observable every BootStrap
    double *bs[N_VAR], value, sum1, sum2, sums[N_VAR], xs[N_VAR];
    struct BootStrapStat stat;

    index = DUMP_FRACTION * (iBLOCK + 1);
    n = iBLOCK - index + 1;

    /*                    |........|...............................|
      a[] index:          0      index                          iBLOCK
      b[] index:                   0     1         ......        n - 1
    */

    for (k = 0; k < N_VAR; k++) {
        bs[k] = & datas[k][index];
        sums[k] = 0.0;
        for (i = 0; i < n; i++) sums[k] += bs[k][i];
        xs[k] = sums[k] / n;
    }

    stat.mean = func(xs);

    for (times = 0; times < BOOTMAX; m *= 2, times++) {
        sum1 = 0.0; sum2 = 0.0;
        for (i = 0; i < m; i++)
        {
            for (k = 0; k < N_VAR; k++)
                sums[k] = 0.0;

            for (j = 0; j < n; j++)
            {
                n1 = (int) (rndm() * n);
                for (k = 0; k < N_VAR; k++)
                    sums[k] += bs[k][n1];
            }

            for (k = 0; k < N_VAR; k++)
                xs[k] = sums[k] / n;

            value = func(xs);
            sum1 += value;
            sum2 += value*value;
        }
        stat.stds[times] = sqrt( sum2 / m - (sum1 / m) * (sum1 / m) );
    }

    stat.n = BOOTMAX;
    return stat;
}
