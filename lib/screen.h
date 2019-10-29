#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define RED     31
#define GREEN   32
#define YELLOW  33
#define BLUE    34
#define MAGENTA 35
#define CYAN    36

#define NORMAL  0
#define BOLD    1

void setcolour(int foreground, int style);
void resetcolour();
void print_success(char* msg);
void print_error(char* msg, int errno_set);

#endif // SCREEN_H_INCLUDED
