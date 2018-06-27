/*-----------------------------------------------------------------------------
 *  header file for errorbar_1_0.c
 *----------------------------------------------------------------------------*/
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

/* ===  FUNCTION  ======================================================================
 *  Description: Implements Nikolay and Boris's analysizing 3/4 print out method
 *  a[] has n elements, a[] = a[n-1]
 * ====================================================================================*/
struct TimeStat ERSTAT(double a[], unsigned int n)
{
    struct TimeStat stat = TimeStat_Init;
    unsigned int i;
    double sum, *ptr = a;
    sum = 0.0;

    for (i = 1; i <= n/4; i++) sum += *ptr++;
    for (i = n/4 + 1; i <= n; i++ )
    {
       sum += *ptr++;
       stat.mean = sum / i;
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
    stat.t_max = (double)(stat.t_max - n/4) / (3 * n /4);
    stat.t_min = (double)(stat.t_min - n/4) / (3 * n /4);
    return (stat);
}

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
struct TimeStat TimeStatistics(double a[], unsigned int datasize)
{
    struct TimeStat stat = TimeStat_Init;
    double *ptr, sum;
    unsigned int index, n, i;
    index = DUMP_FRACTION * datasize;
    n = datasize - index ;

    /*           |........|...............................|
    a index:     0      index                          iBLOCK = datasize - 1
    i index:              1     2            ......       n
    | start |                        n/4+1  ........      n
    |compare|                          1     ......       m
    t_value                         1.0 / m              1.0

    where m = n - n/4 = 3 *n / 4 != 3.0 * n / 4

         ERSTAT is performed for new index in the range [0, 1, ..., n]
         and 0 is discard for counting convenience.
    */

    /*-----------------------------------------------------------------------------
     *  a realization of  ERSTAT(&a[index], n);
     *----------------------------------------------------------------------------*/
    ptr = &a[index];
    sum = 0.0;

    for (i = 1; i <= n/4; i++ ) sum += *ptr++;

    for (i = n/4 + 1; i<=n ; i++)
    {
        sum += *ptr++;
        stat.mean = sum / i;
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
    stat.t_max = (double)(stat.t_max - n/4) / (3 * n /4);
    stat.t_min = (double)(stat.t_min - n/4) / (3 * n /4);
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
 *  Description:  a[datasize] = a[iBLOCK+1]
 * ====================================================================================*/
struct BlockStat BlockStatistics(double a[], unsigned int datasize)
{
    struct BlockStat stat = BlockStat_Init;
    unsigned int i, j, k, index, n, nn;

    index = DUMP_FRACTION * datasize;
    nn = datasize - index;

    /*              |........|...............................|
    a index:        0      index                          iBLOCK = datasize -1
    after dump:             0  1  2         ......       nn - 1  -> calculate mean
    b index:                          0,  1,  2,   ...,   n - 1  -> calculate std
    */

    k = (int) log2(nn + 0.1);   //  2**k <= nn < 2**(k+1)
    n = (int) pow(2, k);        //  n = 2**k

    printf("block # = %u  statistics blocks # = %u \n", datasize, n);
    if ( n < 64) return stat;
    /*-----------------------------------------------------------------------------
      double *ptr, sum;
      sum = 0.0;
      ptr = &a[index];
      for( i = 0; i < nn; i++) sum += ptr[i];
      p->mean = sum / nn;
     *----------------------------------------------------------------------------*/

    double sum, sum2, mean, std;
    double *b, *ptr;
    b = (double *) malloc( n * sizeof(double) );

    ptr = &a[datasize - n];
    for (i = 0; i < n; i++ ) b[i] = ptr[i];

    for (i = 0, sum = 0.0; i < n; i++)
        sum += b[i];

    mean = sum / n;
    stat.mean = mean;

    // merge blocks to calculate the statistics, at least 64 super blocks
    // two consecutive blocks are merged instead of merging a[i] and a[n/2+i] ,

    for (j = 0; ; j++)
    {
        sum2 = 0.0;
        for (i = 0; i < n; i++) sum2 += b[i] * b[i];
        std = sqrt( 1.0/( n*(n-1) ) * (sum2 - n * mean * mean) );
        stat.stds[j] = std;

        k = k - 1;
        n = n / 2;
        if ( n == 32 ) break;

        for (i = 0; i < n; i++)
            b[i] = ( b[2*i] + b[2*i+1] ) / 2.0;

    }
    stat.n = j + 1;   // stds[0], stds[1], ..., stds[j]  ==> (j+1) elements

    free(b);
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
struct BootStrapStat BootStrapStatistics(double a[], unsigned int datasize)
{
    unsigned int m = 256, times = 0;  // m = BootStrapN
    unsigned int i, j, k, index, n;

    // b[] for BootStrap array, average : the average of observable every BootStrap
    double *b, average, sum, sum1, sum2;
    struct BootStrapStat stat;

    index = DUMP_FRACTION * datasize;
    n = datasize - index;

    /*                    |........|...............................|
      a[] index:          0      index                          iBLOCK = datasize - 1
      b[] index:                   0     1         ......        n - 1
    */
    b = &a[index];

    sum = 0.0;
    for (i = 0; i < n; i++) sum += b[i];
    stat.mean = sum / n;

    for (times = 0; times < BOOTMAX; m *= 2, times++) {
        sum1 = 0.0; sum2 = 0.0;
        for (i = 0; i < m; i++)
        {
            sum = 0.0;
            for (j = 0; j < n; j++)
            {
                k = (int) (rndm()*n);
                sum += b[k];
            }
            average = sum / n;
            sum1 += average;
            sum2 += average*average;
        }
        stat.stds[times] = sqrt( sum2 / m - (sum1 / m) * (sum1 / m) );
    }

    stat.n = BOOTMAX;
    return stat;
}

