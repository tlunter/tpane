#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "args.h"
#include "files.h"
#include "terminal.h"

int main(int argc, char **argv) {
    struct file_list_t file_list;
    struct watched_files_t watched_files;

    int parsed = parse_args(argc, argv, &file_list);
    if (!parsed) {
        fprintf(stderr, "Missing file argument\n");
        return 1;
    }

    if (!files_setup(&watched_files, &file_list)) {
        fprintf(stderr, "Failed to read files\n");
        return 1;
    }

    terminal_setup();

    unsigned char buff [100];
    struct timeval last_run = {0};
    struct timeval now;
    struct timeval diff;

    struct watched_files_t recent_files;
    files_alloc(&recent_files, watched_files.file_count);

    while (1) {
        gettimeofday(&now, NULL);
        timersub(&now, &last_run, &diff);
        files_update(&watched_files);

        if (diff.tv_sec > 0 || diff.tv_usec > 500000) {

            terminal_reset_cursor();
            files_get_recent(&watched_files, &recent_files);
            terminal_draw_files(&recent_files);

            last_run = now;
        }

        int characters = terminal_has_input(buff, 100);
        if (characters) {
            if (buff[0] == 3) {
                break;
            }
        }
    }

    terminal_reset();

    return 0;
}
