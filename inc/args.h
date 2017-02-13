#ifndef __ARGS_H__
#define __ARGS_H__

struct file_list_t {
    int file_count;
    char **files;
};

int args_parse(int argc, char **argv, struct file_list_t *file_list);
void args_free(struct file_list_t *file_list);

#endif
