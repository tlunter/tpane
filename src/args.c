#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <glob.h>
#include "args.h"

void parse_args(int argc, char **argv, struct file_list_t *file_list) {
    if (argc <= 1) {
        fprintf(stderr, "Missing file argument\n");
        exit(1);
    }

    int flags = 0;
    int i, n;

    glob_t globbuf;
    for (i = 1; i < argc; i++) {
        glob(argv[i], flags, NULL, &globbuf);
        flags = GLOB_APPEND;
    }

    char **filebuf = (char**)malloc(globbuf.gl_pathc * sizeof(char*));
    for (i = 0; i < globbuf.gl_pathc; i++) {
        n = strlen(globbuf.gl_pathv[i]) + 1;
        filebuf[i] = (char*)malloc(n);
        strcpy(filebuf[i], globbuf.gl_pathv[i]);
    }

    file_list->file_count = globbuf.gl_pathc;
    file_list->files = filebuf;
}
