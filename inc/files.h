#ifndef __FILES_H__
#define __FILES_H__

struct line_t {
    int length;
    char *data;
    struct line_t *prior_line;
    struct line_t *next_line;
};

struct watched_file_t {
    char *file_name;
    unsigned long inode;
    unsigned long modified_time;

    int fd;

    int line_count;
    struct line_t *first_line;
    struct line_t *last_line;
};

struct watched_files_t {
    int file_count;
    struct watched_file_t *files;
};

void files_alloc(struct watched_files_t *watched_files, int file_count);
int files_setup(struct watched_files_t *watched_files, struct file_list_t *file_list);
int files_update(struct watched_files_t *watched_files);
void files_get_recent(struct watched_files_t *watched_files, struct watched_files_t *recent_files);

#endif
