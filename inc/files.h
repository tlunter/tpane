#ifndef __FILES_H__
#define __FILES_H__

struct line_t {
    char *data;
    struct line_t *next_line;
};

struct watched_file_t {
    char *file_name;
    int fd;
    int modified_time;
    int line_count;
    struct line_t *first_line;
    struct line_t *last_line;
};

int files_setup(struct file_list_t *file_list);
void files_print_lines();
int files_update();

#endif
