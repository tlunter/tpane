#ifndef __ARGS_H__
#define __ARGS_H__

struct file_list_t {
    int file_count;
    char **files;
};

int parse_args(int argc, char **argv, struct file_list_t *file_list);

#endif
