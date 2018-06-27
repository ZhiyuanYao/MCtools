/* =====================================================================================
 *
 *       Filename:  test_prototype.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/25/2018 12:13:56
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
struct DataStruc {
    int n;                    /* total number of columns per line */
    int index[10];            /* index of selected data, takes 1, ..., n < 10 */
    char file[256];           /* filepath */
};



int main()
{
    const struct DataStruc DataStruc_Init[] = {
        {2, {2}, "L32_E2.dat"},
        {1, {1}, "L32_E.dat"}
    };

    printf("Length of structure array is : %lu\n",
            sizeof(DataStruc_Init) /sizeof(DataStruc_Init[0]) );

    return 0;

    struct DataStruc *datainfos[2];

    struct DataStruc *infos[] = {
        & (struct DataStruc) {2, {1,2,3,4,5,6,7,8,9,0}, "L32_E2.dat"},
        & (struct DataStruc) {1, {1, 2}, "L32_E.dat"}
    };

    /*-----------------------------------------------------------------------------
     * Two mistakes :
     * 1. The compiler does not know the items are type of DataStruc
     * 2. In initialization array, the initial value must be given, here the value
          is address of pointer to structure

     char *name[] = { "Illegal month", "Jan, "Feb", "Mar" };
     actually "Jan" is address, not string!
     *----------------------------------------------------------------------------*
    struct DataStruc *datainfos[] = {
        {2, {1, 2}, "L32_E2.dat"},
        {1, {1, 2}, "L32_E.dat"}
    };
     *----------------------------------------------------------------------------*/

    /* The const modifier is necessary, otherwise incompatible data  */
    const struct DataStruc *data_pts[] = {
        & DataStruc_Init[0],
        & DataStruc_Init[1]
    };

    /* need to have the type cast, otherwise incompatible data format  */
    datainfos[0] = (struct DataStruc *) DataStruc_Init;

    printf("%p\n", DataStruc_Init[0].index);
    for (int i = 0; i < 10; i++)
        printf("%d\n", DataStruc_Init[0].index[i]);

    for (int i = 0; i < 10; i++)
        printf("%d\n", DataStruc_Init[1].index[i]);

    return 0;
}

