/* =====================================================================================
 *
 *       Filename:  errorbar.c
 *
 *    Description:  This program use the bootstrap method, blocking method, and
 *    NB's time series method to determine the error bar.
 *    Note iBlock = data_size - 1.
 *
 *    analysis of oberservale like susceptibility χ = ( <M2> - <M>^2 ) / (T N) is
 *    added.
 *
 *          Usage:  set N_VAR, func(), and datainfo  in prototype.h
 *                  gcc -lgsl -lm -O2 errorbar.c  && ./a.out
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
#include "gsl_random.h"
#include "utility.h"
#include "linecount.h"
#include "linesplit.h"
#include "prototype.h"

#define MAXLINES 1000000               /* maximum number of lines*/
int main(int argc, char * argv[])
{
    /*--------------------  Random Number Generators --------------------*/
    RNG_ptr = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(RNG_ptr, RNG_seed);
    /*-------------------------------------------------------------------*/
    void help(void), read_data(unsigned long, double **);
    /* void statistics(double **, unsigned long); */
    int counter = argc - 1;

    for (int i = 1; counter; counter--, i++) {
        if ( !strcmp(argv[i], "--help") )  help();
    }

    char *file = (char *) DataInfos[0].file;
    unsigned long datasize = countline(file), iBlock = datasize - 1;
    if ( datasize > MAXLINES) stop("datasize > MAXLINES");

    double *datas[N_VAR];
    for (int i = 0; i < N_VAR; i++)
        datas[i] = (double *) malloc(datasize * sizeof(double));

    read_data(datasize, datas);

    for (int k = 0; k < N_VAR; k++) {
        for (int i = 0; i < 10; i++)
            printf("%f\n", datas[k][i]);
        printf("\n\n");
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
            "Alternatively, one can specify the filepath inside in main program to\n"
            "perform the correponding analysis.\n\n"
          );
    exit(1);
}


#define MAXITEMS  5                    /* maximum number of items per line */
#define MAXLENGTH  100                 /* maximum number of characters per line */
void read_data(unsigned long datasize, double *datas[]) {
    char line[MAXLENGTH], *splits[MAXITEMS];
    for (int i  = 0; i < MAXITEMS; i++) {
        splits[i] = (char *) malloc(MAXLENGTH * sizeof(char));
        splits[i][0] = '\0';
    }

    int nread = 0;
    FILE *fps[N_VAR];
    for (int k = 0; k < N_VAR; ) {
        const int n = DataInfos[k].n;
        char *file = (char *) DataInfos[k].file;
        FILE *fp = fopen(file, "r");

        unsigned long lcount = 0;
        while (fgets(line, sizeof(line), fp) != NULL) {
            linesplits(line, splits);
            for (int i = 0; i < n ; i++) {
                datas[nread + i][lcount] = atof( splits[ DataInfos[k].index[i] ] );
            }
            lcount++;
        }
        fclose(fp);
        k++;
        nread += n;
        if (nread == N_VAR) {
            if ( (unsigned long) k != sizeof(DataInfos) / sizeof(DataInfos[0]) ) {
                printf("reading %d-th item in DataInfos, total items # = %lu \n",
                        k, sizeof(DataInfos) / sizeof(DataInfos[0]));
                stop("error in read data!");
            }
            break;
        }
    }

}
