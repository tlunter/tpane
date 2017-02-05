#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include "terminal.h"

struct termios original, raw;

void terminal_setup() {

    // Save original serial communication configuration for stdin
    tcgetattr( STDIN_FILENO, &original);

    // Put stdin in raw mode so keys get through directly without
    // requiring pressing enter.
    cfmakeraw (&raw);
    tcsetattr (STDIN_FILENO, TCSANOW, &raw);

    // Switch to the alternate buffer screen
    write (STDOUT_FILENO, "\e[?1049h", 8);
}

void terminal_reset() {
    // Revert the terminal back to its original state
    write (STDOUT_FILENO, "\e[?1049l", 8);

    tcsetattr (STDIN_FILENO, TCSANOW, &original);
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
