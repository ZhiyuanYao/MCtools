/* =====================================================================================
 *
 *       Filename:  analyze.c
 *
 *    Description:  Statistic of a single direct observable, set filename[] in the
 *                  program to use. The variable is assumed to be stored as single
 *                  columm in data file (if not include linesplit.h file and modify
 *                  line 49 accordingly).
 *
 *                  Methods : bootstrap, blocking method, NB's time series method
 *
 *        Version:  1.0
 *        Created:  06/26/2018 02:13:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhiyuan Yao, yao.umass@gmail.com
 *   Organization:  University of Massachusetts, Amherst
 *
 * ====================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utility.h"
#include "gsl_random.h"
#include "linecount.h"
#include "statistics_1_0.h"
#define COEFF ( 1.0 / (256.0 * 256.0) )
int main()
{

    /*--------------------  Random Number Generators --------------------*/
    RNG_ptr = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(RNG_ptr, RNG_seed);
    /*-------------------------------------------------------------------*/

    char filename[] = "L16_2.20_E2.dat";
    unsigned long datasize = countline(filename);

    double *data = (double *) malloc( datasize * sizeof(double) );
    if ( data == NULL) stop("error in allocating data.\n");

    unsigned long lc = 0;
    FILE *fp = fopen(filename, "r");
    /*-----------------------------------------------------------------------------
    There is one more lc++ in this construct, since when condition not valid, lc++
    while ( fscanf(fp, "%lf", &data[lc++]) == 1) ;
     *----------------------------------------------------------------------------*/
    while ( fscanf(fp, "%lf", &data[lc]) == 1) lc++;

    if (lc != datasize) {
        printf("lc = %lu datasize = %lu\n", lc, datasize);
        stop("error in loading data.\n");
    }

    /* for (int i = 0; i < 10; i++) */
    /*     printf("%f\n", data[i]); */

    printf("\n------------------------\n");
    printf("BootStrap Statistics\n");
    printf("------------------------\n");
    struct BootStrapStat stat0;
    stat0 = BootStrapStatistics(data, datasize);
    printf("mean = %f\n", stat0.mean * COEFF);
    for (int i = 0; i < stat0.n; i++)
        printf("stds[%d] = %f\n", i, stat0.stds[i] * COEFF);

    printf("\n------------------------\n");
    printf("Time Series Statistics\n");
    printf("------------------------\n");
    struct TimeStat stat;
    stat = TimeStatistics(data, datasize);
    printf(" mean = %f   std = %f\n", stat.mean * COEFF, stat.std * COEFF);
    printf("  max = %f   min = %f\n", stat.max * COEFF, stat.min * COEFF);
    printf("t_max = %f t_min = %f\n", stat.t_max, stat.t_min);

    printf("\n------------------------\n");
    printf("Block Statistics\n");
    printf("------------------------\n");
    struct BlockStat stat1;
    stat1 = BlockStatistics(data, datasize);
    printf("mean = %f\n", stat1.mean * COEFF);
    for (int i = 0; i < stat1.n; i++)
        printf("stds[%d] = %f\n", i, stat1.stds[i] * COEFF);


    fclose(fp);
    return 0;
}

