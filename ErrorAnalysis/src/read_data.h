#define MAXITEMS  5                    /* maximum number of items per line */
#define MAXLENGTH  100                 /* maximum number of characters per line */
void read_data(double *datas[], unsigned long datasize) {
    char line[MAXLENGTH], *splits[MAXITEMS];
    for (int i  = 0; i < MAXITEMS; i++) {
        splits[i] = (char *) malloc(MAXLENGTH * sizeof(char));
        splits[i][0] = '\0';
    }

    int nread = 0;
    for (int k = 0; k < N_VAR; ) {
        const int n = DataInfos[k].n;
        char *file = (char *) DataInfos[k].file;
        FILE *fp = fopen(file, "r");

        unsigned long lcount = 0;
        while (fgets(line, sizeof(line), fp) != NULL) {
            linesplits(line, splits);
            for (int i = 0; i < n ; i++) {
                datas[nread + i][lcount] = atof( splits[ DataInfos[k].index[i] -1] );
            }
            lcount++;
        }
        if ( datasize != lcount) stop("error in read_data\n");
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
