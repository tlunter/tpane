#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "args.h"
#include "files.h"
#include "terminal.h"

int main(int argc, char **argv) {
    struct file_list_t file_list;

    int parsed = parse_args(argc, argv, &file_list);
    if (!parsed) {
        fprintf(stderr, "Missing file argument\n");
        exit(1);
    }

    terminal_setup();

    for (int i = 0; i < file_list.file_count; i++) {
        fprintf(stderr, "File: %s\r\n", file_list.files[i]);
    }
    if (file_list.file_count > 0) {
        fprintf(stderr, "\r\n");
    }

    if (!files_setup(&file_list)) {
        terminal_reset();

        fprintf(stderr, "Failed to read files\n");
        return 1;
    }

    struct winsize w;
    unsigned char buff [100];

    while (1) {
        ioctl(0, TIOCGWINSZ, &w);
        //printf("Lines %d\r\n", w.ws_row);
        //printf("Columns %d\r\n", w.ws_col);

        files_update();
        files_print_lines();

        int characters = terminal_has_input(buff, 100);
        if (characters) {
            if (buff[0] == 3) {
                terminal_reset();

                fprintf(stderr, "Dying\n");
                return 0;
            }
        }
    }

    terminal_reset();

    return 0;
}
