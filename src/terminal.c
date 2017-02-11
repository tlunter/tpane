#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include "args.h"
#include "files.h"
#include "terminal.h"

struct termios original, raw;

void terminal_clear_screen() {
    write (STDOUT_FILENO, "\e[2J", 5);
}

void terminal_clear_down() {
    write (STDOUT_FILENO, "\e[0J", 5);
}

void terminal_clear_rest_of_line() {
    write (STDOUT_FILENO, "\e[0K", 7);
    printf("\r\n");
}

void terminal_setup() {
    // Save original serial communication configuration for stdin
    tcgetattr( STDIN_FILENO, &original);

    // Put stdin in raw mode so keys get through directly without
    // requiring pressing enter.
    cfmakeraw (&raw);
    tcsetattr (STDIN_FILENO, TCSANOW, &raw);

    // Switch to the alternate buffer screen
    write (STDOUT_FILENO, "\e[?1049h", 8);
    terminal_clear_screen();
}

void terminal_reset() {
    // Revert the terminal back to its original state
    write (STDOUT_FILENO, "\e[?1049l", 8);

    tcsetattr (STDIN_FILENO, TCSANOW, &original);
}

void terminal_set_cursor(int x, int y) {
    dprintf(STDOUT_FILENO, "\e[%d;%dH", x, y);
}

void terminal_reset_cursor() {
    terminal_set_cursor(1, 1);
}

int terminal_has_input(unsigned char *buff, int size) {
    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    retval = select(1, &rfds, NULL, NULL, &tv);

    if (retval) {
        return read (STDIN_FILENO, buff, size);
    }

    return 0;
}

void terminal_get_size(int *height, int *width) {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    *height = w.ws_row;
    *width = w.ws_col;
}

int terminal_lines_per_pane(int height, int panes) {
    return (height / panes) - (int)ceil(((double)panes - 1) / (double)panes);
}

void terminal_draw_in_pane(int starting_height, int starting_width, int height, int width, struct watched_file_t file) {
    struct line_t *line = file.last_line;

    int lines_counted = 0;

    while (line != NULL) {
        int lines_for_current_line = ceil((double)line->length / (double)width);
        lines_counted += lines_for_current_line;

        if (line->prior_line == NULL) {
            break;
        } else if (lines_counted > height) {
            line = line->next_line;
            break;
        } else if (lines_counted == height) {
            lines_counted = height;
            break;
        } else {
            line = line->prior_line;
        }
    }

    terminal_set_cursor(starting_height, starting_width);

    int current_height = starting_height;
    while (line != NULL) {
        for (int i = 0; i < line->length; i++) {
            if (i != 0 && (i % width) == 0) {
                terminal_set_cursor(++current_height, starting_width);
            }
            write(STDOUT_FILENO, line->data + i, 1);
        }
        for (int i = line->length; i < width; i++) {
            write(STDOUT_FILENO, " ", 1);
        }
        terminal_set_cursor(++current_height, starting_width);
        line = line->next_line;
    }
    while (current_height < starting_height + height) {
        for (int i = 0; i < width; i++) {
            write(STDOUT_FILENO, " ", 1);
        }
        terminal_set_cursor(++current_height, starting_width);
    }
}

void terminal_draw_borders(int pane_count, int lines_per_pane, int width) {
    if (pane_count <= 1) {
        return;
    }

    for (int i = 1; i < pane_count; i++) {
        terminal_set_cursor(i * (lines_per_pane + 1), 1);
        for (int j = 0; j < width; j++) {
            write(STDOUT_FILENO, "=", 1);
        }
    }
}

void terminal_draw_files(struct watched_files_t *watched_files) {
    int height, width;
    terminal_get_size(&height, &width);

    int pane_count = watched_files->file_count;
    if (!pane_count) {
        terminal_clear_screen();
        return;
    }

    int lines_per_pane = terminal_lines_per_pane(height, pane_count);

    terminal_draw_borders(pane_count, lines_per_pane, width);

    for (int i = 0; i < watched_files->file_count; i++) {
        struct watched_file_t file = watched_files->files[i];

        if (i == watched_files->file_count - 1) {
        }
        terminal_draw_in_pane(i * (lines_per_pane + 1) + 1, 1, lines_per_pane, width, file);
    }

    terminal_clear_down();
    terminal_set_cursor(height, 1);
}
