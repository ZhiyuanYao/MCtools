 /* ====================================================================================
 *
 *       Filename:  errorbar.c
 *
 *    Description:  This program use the bootstrap method, blocking method, and
 *    NB's time series method to determine the error bar. The variable is assumed
 *    to be stored as single colomb in data file (if not include linesplic.c file
 *    and modify line 50 accordingly). We use the countline function in linecount.c
 *    to determine the size of the data, allocate array of correponding length.
 *
 *    Compared to 1.1, the main() is made to be more structured, and statistics
 *    analysis of oberservale like susceptibility χ = ( <M2> - <M>^2 ) / (T N) is
 *    added.
 *
 *          Usage:  gcc -lgsl -lm -O2 errorbar.c -o errorbar
 *                  > errorbar file
 *                  > errorbar file2 file
 *
 *        Version:  2.0
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
#include "gsl_random.h"
#include "statistics_2_0.h"
#include "linecount.h"
#define MAXLINES 1000000
int main(int argc, char * argv[])
{
    /*--------------------  Random Number Generators --------------------*/
    RNG_ptr = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(RNG_ptr, RNG_seed);
    /*-------------------------------------------------------------------*/
    void help(void), statistics(char *), statistics1(char *), statistics2(char *arg2, char *arg);
    int counter = argc - 1;

    for (int i = 1; counter; counter--, i++) {
        if ( !strcmp(argv[i], "--help") )  help();
    }

    if (argc == 1) {
        help();
    }
    else if (argc == 2)
        statistics(argv[1]);
    else if (argc == 3)
        statistics2(argv[1], argv[2]);
    else {
        printf("Wrong argument number\n");
        help();
        return 1;
    }

    return 0;
}

/* ===  FUNCTION  ======================================================================
 *  Description: The helper message for --help flag
 * ====================================================================================*/
void help(void) {
    printf("\nThis program is intended to calculate the statistics of observable O\n"
           "by various methods.\n"
           "      If <O> = <M>,          then ./a.out M_filepath \n"
           "      If <O> = <M2> - <M>^2, then ./a.out M2_filepath M_filepath \n"
           "Note that the data file is assumed to be stored as single column.\n \n"
          );
    exit(1);
}

/* ===  FUNCTION  ======================================================================
 *  Description:  Here we calculate O = <M> and it's assumed that the M is stored
 *  in file as single colomn.
 * ====================================================================================*/
void statistics(char *file) {
    unsigned long lcount = countline(file), datasize = lcount;
    if (lcount > MAXLINES ) {
        printf("data size exceeds number of maximum lines %d\n", MAXLINES);
        exit(1);
    }

    double *data = (double *) malloc( lcount * sizeof(double));
    if ( data == NULL)  {
        printf("allocation of data array failed!\n");
        exit(1);
    }

    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        printf("no such file.");
        exit(1);
    }

    lcount = 0;
    while (fscanf(fp,"%lf", &data[lcount++]) == 1) ;
    /* The %*s, %*f, %*d  can be  used to ignore some input, for example the format
                 E = 0.75
                 E = 0.78
                 ......
       can  be read into the data[] by
            while (fscanf(pt,"%*s %*s %f ", data[lcount++]) == 1) ;
    */

    printf("\n------------------------\n");
    printf("BootStrap Statistics\n");
    printf("------------------------\n");
    struct BootStrapStat stat0;
    stat0 = BootStrapStatistics(data, datasize);
    printf("mean = %f\n", stat0.mean);
    for (int i = 0; i < stat0.n; i++)
        printf("stds[%d] = %f\n", i, stat0.stds[i]);

    printf("\n------------------------\n");
    printf("Time Series Statistics\n");
    printf("------------------------\n");
    struct TimeStat stat;
    stat = TimeStatistics(data, datasize);
    printf(" mean = %f   std = %f\n", stat.mean, stat.std);
    printf("  max = %f   min = %f\n", stat.max, stat.min);
    printf("t_max = %f t_min = %f\n", stat.t_max, stat.t_min);

    printf("\n------------------------\n");
    printf("Block Statistics\n");
    printf("------------------------\n");
    struct BlockStat stat1;
    stat1 = BlockStatistics(data, datasize);
    printf("mean = %f\n", stat1.mean);
    for (int i = 0; i < stat1.n; i++)
        printf("stds[%d] = %f\n", i, stat1.stds[i]);

    printf("\n");
    free(data);
}


/* ===  FUNCTION  ======================================================================
 *  Description:  Here we calculate O = <M2> -<M>^2  and it's assumed that the
 *  M2 is stored in file2, M is stored in file as a single colomn.
 * ====================================================================================*/
void statistics2(char *file2, char*file) {
    unsigned long lcount = countline(file), datasize = lcount;
    if (lcount > MAXLINES ) {
        printf("data size exceeds number of maximum lines %d\n", MAXLINES);
        exit(1);
    }

    double *data = (double *) malloc( lcount * sizeof(double));
    double *data2 = (double *) malloc( lcount * sizeof(double));
    if ( data == NULL || data2 == NULL ) {
        printf("allocation of data, data2 array failed!\n");
        exit(1);
    }

    FILE *fp = fopen(file, "r");
    FILE *fp2 = fopen(file2, "r");
    if (fp == NULL || fp2 == NULL) {
        printf("no such file.");
        exit(1);
    }

    lcount = 0;
    while (fscanf(fp,"%lf", &data[lcount++]) == 1) ;
    lcount = 0;
    while (fscanf(fp2,"%lf", &data2[lcount++]) == 1) ;

    printf("\n------------------------\n");
    printf("BootStrap Statistics2\n");
    printf("------------------------\n");
    struct BootStrapStat stat0;
    stat0 = BootStrapStatistics2(data2, data, datasize);
    printf("mean = %f\n", stat0.mean);
    for (int i = 0; i < stat0.n; i++)
        printf("stds[%d] = %f\n", i, stat0.stds[i]);

    printf("\n------------------------\n");
    printf("Time Series Statistics2\n");
    printf("------------------------\n");
    struct TimeStat stat;
    stat = TimeStatistics2(data2, data, datasize);
    printf(" mean = %f   std = %f\n", stat.mean, stat.std);
    printf("  max = %f   min = %f\n", stat.max, stat.min);
    printf("t_max = %f t_min = %f\n", stat.t_max, stat.t_min);

    printf("\n------------------------\n");
    printf("Block Statistics2\n");
    printf("------------------------\n");
    struct BlockStat stat1;
    stat1 = BlockStatistics2(data2, data, datasize);
    printf("mean = %f\n", stat1.mean);
    for (int i = 0; i < stat1.n; i++)
        printf("stds[%d] = %f\n", i, stat1.stds[i]);

    printf("\n");
    free(data);
    free(data2);
}
