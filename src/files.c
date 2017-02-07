#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "args.h"
#include "files.h"

int file_count;
struct watched_file_t *files;

struct line_t *line_alloc(char *payload, int length) {
    char *data = (char*)malloc(length + 1);
    strncpy(data, payload, length);

    struct line_t *line = (struct line_t*)malloc(sizeof(struct line_t));
    line->data = data;

    return line;
}

void line_dealloc(struct line_t *line) {
    free(line->data);
    free(line);
}

int files_setup(struct file_list_t *file_list) {
    file_count = file_list->file_count;
    files = (struct watched_file_t*)malloc(file_list->file_count * sizeof(struct watched_file_t));

    for (int i = 0; i < file_list->file_count; i++) {
        int n = strlen(file_list->files[i]) + 1;
        files[i].file_name = (char*)malloc(n);
        strcpy(files[i].file_name, file_list->files[i]);

        int fd = open(file_list->files[i], O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            return 0;
        }

        files[i].fd = fd;
    }

    return 1;
}

int files_modified_time(char *file_name) {
    struct stat attrib;
    stat(file_name, &attrib);

    return attrib.st_mtim.tv_sec;
}

void files_read_lines(struct watched_file_t *file) {
    int line_position;
    char line_buffer[1024];

    int amount_read;
    char read_buffer[1024];

    do {
        line_position = 0;
        amount_read = read(file->fd, read_buffer, 1024);

        for (int i = 0; i < amount_read; i++) {
            if (read_buffer[i] == 10) {
                struct line_t *line = line_alloc(line_buffer, line_position);

                if (file->first_line == NULL) {
                    file->line_count = 1;
                    file->first_line = line;
                    file->last_line = line;
                } else {
                    if (file->line_count >= 100) {
                        line_dealloc(file->first_line);
                        file->first_line = file->first_line->next_line;
                    } else {
                        file->line_count++;
                    }
                    file->last_line = file->last_line->next_line = line;
                }
                line_position = 0;
            } else {
                line_buffer[line_position++] = read_buffer[i];
            }
        }
    } while (amount_read > 0);
}

void files_print_lines() {
    for (int i = 0; i < file_count; i++) {
        struct watched_file_t file = files[i];
        struct line_t *line = file.first_line;
        while (line != NULL) {
            printf("%s\r\n", line->data);
            line = line->next_line;
        }
    }
}

int files_update() {
    for (int i = 0; i < file_count; i++) {
        files[i].modified_time = files_modified_time(files[i].file_name);
        files_read_lines(files+i);
    }

    return 1;
}
