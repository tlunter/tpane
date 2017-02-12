#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "args.h"
#include "files.h"

#define MAX_MODIFIED_TIME 300

struct line_t *line_alloc(char *payload, int length) {
    char *data = (char*)malloc(length + 1);
    strncpy(data, payload, length);
    data[length + 1] = '0';

    struct line_t *line = (struct line_t*)malloc(sizeof(struct line_t));
    line->length = length;
    line->data = data;
    line->next_line = NULL;
    line->prior_line = NULL;

    return line;
}

void line_dealloc(struct line_t *line) {
    free(line->data);
    free(line);
}

void files_alloc(struct watched_files_t *watched_files, int file_count) {
    watched_files->file_count = file_count;
    watched_files->files = (struct watched_file_t*)malloc(file_count * sizeof(struct watched_file_t));
}

int files_setup(struct watched_files_t *watched_files, struct file_list_t *file_list) {
    files_alloc(watched_files, file_list->file_count);

    for (int i = 0; i < file_list->file_count; i++) {
        int n = strlen(file_list->files[i]) + 1;
        watched_files->files[i].file_name = (char*)malloc(n);
        strcpy(watched_files->files[i].file_name, file_list->files[i]);

        int fd = open(file_list->files[i], O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            return 0;
        }

        watched_files->files[i].fd = fd;
    }

    return 1;
}

int files_update_stat(struct watched_file_t *file) {
    int retval;

    struct stat attrib;
    stat(file->file_name, &attrib);

    file->modified_time = attrib.st_mtime;

    if (attrib.st_ino != file->inode && file->inode != 0) {
        retval = 1;
    } else {
        retval = 0;
    }
    file->inode = attrib.st_ino;

    return retval;
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
                        file->first_line->prior_line = NULL;
                    } else {
                        file->line_count++;
                    }

                    file->last_line->next_line = line;
                    line->prior_line = file->last_line;

                    file->last_line = line;
                }
                line_position = 0;
            } else {
                line_buffer[line_position++] = read_buffer[i];
            }
        }
    } while (amount_read > 0);
}

void files_reopen_file(struct watched_file_t *file) {
    file->fd = open(file->file_name, O_RDONLY | O_NONBLOCK);
    lseek(file->fd, 0, SEEK_END);
}

int files_update(struct watched_files_t *watched_files) {
    for (int i = 0; i < watched_files->file_count; i++) {
        if (files_update_stat(watched_files->files + i)) {
            files_reopen_file(watched_files->files + i);
        }
        files_read_lines(watched_files->files + i);
    }

    return 1;
}

void files_get_recent(struct watched_files_t *watched_files, struct watched_files_t *recent_files) {
    int file_count = 0;
    time_t now = time(0);

    if (now < 0) {
        return;
    }

    for (int i = 0; i < watched_files->file_count; i++) {
        struct watched_file_t file = watched_files->files[i];

        if ((now - file.modified_time) < MAX_MODIFIED_TIME) {
            memcpy((recent_files->files + file_count), (watched_files->files + i), sizeof(struct watched_file_t));
            file_count++;
        }
    }

    recent_files->file_count = file_count;
}
