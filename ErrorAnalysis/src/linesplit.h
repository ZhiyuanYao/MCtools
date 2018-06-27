#include <string.h>
/* ===  FUNCTION  ======================================================================
 *  Description:  Python style split() function, n takes values from 0, the
 *  argument string = line.split()[n]
 * ====================================================================================*/
void linesplit0(char *string, const char *line, int n)
{
    int i, j, k = 0;

    for (i = 0; i < (int) strlen(line); i++) {
        if ( line[i] != ' ' && line[i] != '\t' && line[i] != '\n') {
            j = 0;
            do {
                if (k == n) string[j++] = line[i];
                i++;
            } while ( line[i] != ' ' && line[i] != '\t' && line[i] != '\n');
            if ( k == n) {
                string[j] = '\0';
                break;
            }
            k++;
        }
    }
}

/* ===  FUNCTION  ======================================================================
 *  Description:   Python style split() function, n takes values from 1, the
 *  argument string = line.split()[n]
 * ====================================================================================*/
void linesplit1(char *string, const char *line, int n)
{
    int i, j, k;

    for (i = 0, k = 1; i < (int) strlen(line); i++) {
        if ( line[i] != ' ' && line[i] != '\t' && line[i] != '\n') {
            j = 0;
            do {
                if (k == n) string[j++] = line[i];
                i++;
            } while ( line[i] != ' ' && line[i] != '\t' && line[i] != '\n');
            if ( k == n) {
                string[j] = '\0';
                break;
            }
            k++;
        }
    }
}

/* ===  FUNCTION  ======================================================================
 *  Description:  splits[] is a pointer array to the splitted items of a line
 * ====================================================================================*/
void linesplits(const char *line, char *splits[])
{
    int i, j, k;

    for (i = 0, k = 0; i < (int) strlen(line); i++) {
        if ( line[i] != ' ' && line[i] != '\t' && line[i] != '\n') {
            j = 0;
            do {
                splits[k][j++] = line[i];
                i++;
            } while ( line[i] != ' ' && line[i] != '\t' && line[i] != '\n');
            splits[k][j] = '\0';
            k++;
        }
    }
}

/*-----------------------------------------------------------------------------
 *  Description: Test Driver Program for linesplit() function
 ----------------------------------------------------------------------------*/
#if 0
#include <stdio.h>
#include <stdlib.h>
#define MAXITEMS  5    /* maximum number of items per line */
int main() {
    char line[] = " 78.9     90  13  +- ,";
    char string[100];
    char *splits[MAXITEMS];
    for (int i  = 0; i < MAXITEMS; i++) {
        splits[i] = (char *) malloc(100 * sizeof(char));
        splits[i][0] = '\0';
    }

    linesplits(line, splits);
    for (int i = 0; i < MAXITEMS; i++)
        printf("%s \n", splits[i]);

    for (int i = 1; i < 4; i++) {
        linesplit1(string, line, i);
        printf("%s\n", string);
    }
    return 0;
}
#endif
