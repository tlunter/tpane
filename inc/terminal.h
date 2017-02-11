#ifndef __DRAWING_H__
#define __DRAWING_H__

void terminal_setup();
void terminal_reset();
void terminal_reset_cursor();
int terminal_has_input(unsigned char *buff, int size);
void terminal_draw_files(struct watched_files_t *watched_files);

#endif
