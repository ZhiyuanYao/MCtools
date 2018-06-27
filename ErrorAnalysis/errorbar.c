/* =====================================================================================
 *
 *       Filename:  errorbar.c
 *
 *    Description:  Dtermine the error bar of a physical variable Q that is a
 *                  funcion of direct observables. For example, Q can be
 *                  susceptibility
 *
 *                             Q = ( <M2> - <M>^2 ) / (T N)
 *
 *                  Use --help flag for detailed instructions.
 *
 *                  Methods : bootstrap, blocking method, NB's time series method
 *
 *          Usage:  set N_VAR, func(), and DataInfos[] in parameter.h
 *                  gcc -lgsl -lm -O2 errorbar.c -o errorbar && ./errorbar
 *
 *        Version:  3.0
 *        Created:  06/22/2018 14:44:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhiyuan Yao, yao.umass@gmail.com
 *   Organization:  University of Massachusetts, Amherst
 *
 * ====================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "src/gsl_random.h"
#include "src/utility.h"
#include "src/linecount.h"
#include "src/linesplit.h"
#include "parameter.h"
#include "src/read_data.h"
#include "src/statistics.h"
#include "src/help.h"

#define MAXLINES 1000000               /* maximum number of lines (datasize) */
int main(int argc, char * argv[])
{
    /*--------------------  Random Number Generators --------------------*/
    RNG_ptr = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(RNG_ptr, RNG_seed);
    /*-------------------------------------------------------------------*/
    void analyze(double *datas[], unsigned long );

    int counter = argc - 1;

    for (int i = 1; counter; counter--, i++) {
        if ( !strcmp(argv[i], "--help") )  help();
    }

    char *file = (char *) DataInfos[0].file;
    unsigned long datasize = countline(file);
    if ( datasize > MAXLINES) stop("datasize > MAXLINES");

    double *datas[N_VAR];
    for (int i = 0; i < N_VAR; i++) {
        datas[i] = (double *) malloc(datasize * sizeof(double));
        if ( datas[i] == NULL)  {
            printf("allocation of datas[%d] array failed!\n", i);
            exit(1);
        }
    }

    read_data(datas, datasize);
    analyze(datas, datasize);

    for (int i = 0; i < N_VAR; i++) free(datas[i]);
    return 0;
}


void analyze(double *datas[], unsigned long datasize)
{
    printf("\n------------------------\n");
    printf("BootStrap Statistics\n");
    printf("------------------------\n");
    struct BootStrapStat stat0;
    stat0 = BootStrapStatistics(datas, datasize);
    printf("mean = %f\n", stat0.mean * COEFF);
    for (int i = 0; i < stat0.n; i++)
        printf("stds[%d] = %f\n", i, stat0.stds[i] * COEFF);

    printf("\n------------------------\n");
    printf("Time Series Statistics\n");
    printf("------------------------\n");
    struct TimeStat stat;
    stat = TimeStatistics(datas, datasize);
    printf(" mean = %f   std = %f\n", stat.mean * COEFF, stat.std * COEFF);
    printf("  max = %f   min = %f\n", stat.max * COEFF, stat.min * COEFF);
    printf("t_max = %f t_min = %f\n", stat.t_max, stat.t_min);

    printf("\n------------------------\n");
    printf("Block Statistics\n");
    printf("------------------------\n");
    struct BlockStat stat1;
    stat1 = BlockStatistics(datas, datasize);
    printf("mean = %f\n", stat1.mean * COEFF);
    for (int i = 0; i < stat1.n; i++)
        printf("stds[%d] = %f\n", i, stat1.stds[i] * COEFF);

    printf("\n");
}

