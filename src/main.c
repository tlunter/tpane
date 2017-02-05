#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "args.h"

int main(int argc, char **argv) {
    struct file_list_t file_list;

    parse_args(argc, argv, &file_list);

    for (int i = 0; i < file_list.file_count; i++) {
        fprintf(stderr, "File: %s\n", file_list.files[i]);
    }
    if (file_list.file_count > 0) {
        fprintf(stderr, "\n");
    }

    struct winsize w;

    while (1) {
        ioctl(0, TIOCGWINSZ, &w);
        printf("Lines %d\n", w.ws_row);
        printf("Columns %d\n", w.ws_col);

        sleep(1);
    }

    return 0;
}
