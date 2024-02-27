#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define HIDE_CURSOR "\e[?25l"
#define SHOW_CURSOR "\e[?25h"
#define RESET_COLOR "\e[m"
#define ESC '\033'

static struct termios old_termios, new_termios;

void reset_terminal(void);
void config_terminal(void);
int read_input(void);
int read_key(char *buffer, int k);
void print_key(int key);

int main(void) {
    config_terminal();
    struct timespec req = {};
    struct timespec rem = {};

    while(1) {
        int key = read_input();
        print_key(key);

        req.tv_nsec = 0.1 * 1000000000; // 0.1 seconds
        nanosleep(&req, &rem);
    }
}

void reset_terminal() {
    printf(RESET_COLOR);
    printf(SHOW_CURSOR);
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

void config_terminal() {
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;

    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    printf(HIDE_CURSOR);
    atexit(reset_terminal);
}

int read_key(char *buffer, int k) {
    if(buffer[k] == ESC && buffer[k+1] == '[') {
        switch(buffer[k+2]) {
            case 'A': return 1; // UP
            case 'B': return 2; // DOWN
            case 'C': return 3; // RIGHT
            case 'D': return 4; // LEFT
        }
    }
    return 0;
}


int read_input(void) {
    char buffer[4096];
    int n = read(STDIN_FILENO, buffer, sizeof(buffer));
    int final_key = 0;
    for(int k = 0; k <= n-3; k+= 3) {
        int key = read_key(buffer, k);
        if(key == 0) continue;
        final_key = key;
    }
    return final_key;
}

void print_key(int key) {
    if(key == 1) printf("Up\n");
    if(key == 2) printf("Down\n");
    if(key == 3) printf("Right\n");
    if(key == 4) printf("Left\n");
}
