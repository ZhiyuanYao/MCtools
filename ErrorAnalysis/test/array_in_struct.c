#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct DataInfo {
    int n;
    int index[10];    /* int index[n]; is wrong */
    char line[100];
};

int main()
{
    struct DataInfo datainfo = {5, {1, 3, 5, 7, 9}, "Hello World!\n"};
    struct DataInfo *pt = &datainfo;
    int *ptr = pt->index;

    char *line_pt = pt->line;

    char message[] = "Haha!\n";
    char *char_pt = message;

    printf("Haha!\n");
    printf("%s", char_pt);
    printf("%s", message);


    /* ===  Mistakes  ======================================================================
     When using printf, the format string is better be a string literal and not a variable
     printf(message);
     printf(char_pt);
     * ====================================================================================*/

    for (int i = 0; i < pt->n; i++)
        printf("%d\n", ptr[i]);

    return 0;
}

