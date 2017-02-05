#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include "args.h"
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

    struct winsize w;
    unsigned char buff [100];

    time_t last_loop = time(0);
    time_t now = last_loop;

    while (1) {
        now = time(0);
        if ((now - last_loop) > 1) {
            last_loop = now;

            ioctl(0, TIOCGWINSZ, &w);
            printf("Lines %d\r\n", w.ws_row);
            printf("Columns %d\r\n", w.ws_col);
        }

        int characters = terminal_has_input(buff, 100);
        if (characters) {
            if (buff[0] == 3) {
                fprintf(stderr, "Dying\r\n");
                break;
            } else {
                printf("characters: %d buffer: ", characters);
                for (int i = 0; i < characters; i++) {
                    printf("%x", buff[i]);
                }
                printf("\r\n");
            }
        }
    }

    terminal_reset();

    return 0;
}
