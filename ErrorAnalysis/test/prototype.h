/* ===  FUNCTION  ======================================================================
 *  Description: n = N_VAR is the number of the variables of the function, n is useful
 *  for subroutines in statistics.h to allocate sums[n] and sum2s[n];
   var[0], var[1], var[2], ..., var[n-1] are the variables.
 * ====================================================================================*/
#define N_VAR 2
inline double func(double var[])
{
    double M = var[0], M2 = var[1];
    return (M2 - M * M);
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
 *
 *  The purpose of this setting is to increase the performance of reading data
 *  from files : a single data file will be loaded only once and all the data
 *  contained in the file will be read into the system.
 *----------------------------------------------------------------------------*/
// const struct DataStruc DataInfos[] = {
//     {1, {1}, "L16_2.20_E.dat"},
//     {1, {1}, "L16_2.20_E.dat"},
// };

const struct DataStruc DataInfos[] = {
    {2, {1, 1}, "L16_2.20_E.dat"},
};
