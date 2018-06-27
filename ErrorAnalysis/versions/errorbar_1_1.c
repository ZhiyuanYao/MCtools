/* =====================================================================================
 *
 *       Filename:  errorbar.c
 *
 *    Description:  Statistic of a single direct observable, set filepath in command
 *                  line to use. The variable is assumed to be stored as single
 *                  columm in data file (if not include linesplit.h file and modify
 *                  line 58 accordingly).
 *
 *                  Methods : bootstrap, blocking method, NB's time series method
 *
 *          Usage:  gcc -lgsl -lm -O2 errorbar.c -o errorbar
 *                  errorbar pathname
 *
 *        Version:  1.1
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
#include <math.h>
#include "gsl_random.h"
#include "statistics_1_0.h"
#include "linecount.h"
#define MAXLINES 1000000
#define COEFF ( 1.0 / (256.0 * 256.0) )
int main(int argc, char * argv[])
{
    /*--------------------  Random Number Generators --------------------*/
    RNG_ptr = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(RNG_ptr, RNG_seed);
    /*-------------------------------------------------------------------*/
    if (argc != 2)
        (argc < 2) ? printf("missing file path!\n") : printf("too many args!\n");

    unsigned long count;
    unsigned long datasize = countline(argv[1]);
    if (datasize > MAXLINES ) {
        printf("data size exceeds number of maximum lines %d\n", MAXLINES);
        return 1;
    }
    double *data = (double *) malloc( datasize * sizeof(double));
    if ( data == NULL) {
        printf("allocation of data array failed!\n");
        return 1;
    }
    FILE *fp = fopen(argv[1], "r");
    /* strlen(line) has to be bigger than # of characters per line in the source file */
    char line[500];

    count = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        data[count++] = atof(line);
    }

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

    printf("\n");
    free(data);
    return 0;
}

