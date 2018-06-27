/* =====================================================================================
 *
 *       Filename:  line.c
 *
 *    Description:  Adaptted from the BSD wc program, correct result even for pdf
 *    pdf, binary files etc, and faster than system call wc - l
 *
 *    https://opensource.apple.com/source/text_cmds/text_cmds-87/wc/wc.c.auto.html
 *
 *        Version:  2.0
 *        Created:  06/22/2018 15:30:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zhiyuan Yao, yao.umass@gmail.com
 *   Organization:  University of Massachusetts, Amherst
 *
 * ====================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>    /* header file for open  */
#include <unistd.h>   /* header file for read, write, close  */
#ifndef BUFFSIZE
#define BUFFSIZE (1024 * 8)
#endif
unsigned long countline(char *file)
{
    char buffer[BUFFSIZE];
    char *p = buffer;
    int fd, len;
    unsigned long lcount = 0;

    if ( (fd = open(file, O_RDONLY, 0)) < 0 ) {
        printf("cannot open file `%s'\n", file);
        exit(1);
    }

    while ( (len = read(fd, buffer, BUFFSIZE)) ) {
        if (len == -1) {
            printf("system read error\n");
            close(fd);
            exit(1);
        }
        for (p = buffer; len--; ++p)
            if (*p == '\n')
                ++lcount;
    }

    close(fd);
    return lcount;
}

/*-----------------------------------------------------------------------------
 *  Description: Test Driver Program for countline() function
 ------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    printf("line count = %lu\n", countline(argv[1]));
    return 0;
}
*----------------------------------------------------------------------------*/
