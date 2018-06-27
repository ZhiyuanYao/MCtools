/* ===  FUNCTION  ======================================================================
 *  Description: n = N_VAR is the number of the variables of the function, n is useful
 *  for subroutines in statistics.h to allocate sums[n] and sum2s[n];
   var[0], var[1], var[2], ..., var[n-1] are the variables.

   The final result also contains an extra factor COEFF
 * ====================================================================================*/
#define N_VAR 2
#define N 256.0
#define T 2.20
// #define COEFF (1.0 / (T*N) )
#define COEFF 1.0
/*-----------------------------------------------------------------------------
 *  do NOT use inline flag for func(), otherwise linker will fail for subroutines
 *  in statistics.h
 *----------------------------------------------------------------------------*/
double func(double var[N_VAR])
{
    /*-----------------------------------------------------------------------------
     *  For clarity one can first write down the correspondence of the symbols
     *  with var[], and then define the functions in terms of symbols. With
     *  optimization flags, the intuitive method

                    double M = var[0], M2 = var[1];
                    return (M2 - M * M);
       and
                    return ( var[1] - var[0]*var[0] );
       should have the same performance.
     *----------------------------------------------------------------------------*/
    return ( var[1] - var[0]*var[0] );
}

struct DataStruc {
    int n;                    /* total number of columns per line */
    int index[10];            /* index of selected data, takes 1, ..., n < 10 */
    char file[256];           /* filepath */
};

/*-----------------------------------------------------------------------------
 *  pointer array to DataStruc
 *  The format of k-th item is {n, i_1,..., i_n, filepath} where n is the number of
 *  variables to be selected from each line, and i_j is the colomn position of
 *  each selected variable.
 *
 *  Note the sequence of datainfos is important. For example, i_1, i_2, ..., i_n
 *  corresponds to var[m+1], var[k+2], ..., var[m+n] where m is the offset
 *  determined by previous datainfo items before to this specific one,
 *     m = \sum_{i=1}^{k-1} n_i
 *  and the starting value of i_ is 1 (corresponds to first columns)
 *
 *  The purpose of this setting is to increase the performance of reading data
 *  from files : a single data file will be loaded only once and all the data
 *  contained in the file will be read into the system.
 *----------------------------------------------------------------------------*/

const struct DataStruc DataInfos[] = {
    {1, {1}, "data/L16_2.20_E.dat"},
    {1, {1}, "data/L16_2.20_E2.dat"}
};
