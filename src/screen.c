/*
** screen.c
** Library regrouping screen-based functions
** ------------------------------------------
** Made by Gilles Henrard
** Last modified : 29/10/2019
*/
#include "screen.h"

/************************************************************************/
/*  I : foreground colour code                                          */
/*      style (normal, bold, ...) of the output to be displayed         */
/*  P : modifies the terminal text output to the chosen colour          */
/*  O : /                                                               */
/************************************************************************/
void setcolour(int foreground, int style)
{
    printf("\033[%d;%dm", style, foreground);
}

/************************************************************************/
/*  I : /                                                               */
/*  P : resets the original colours of the standard output              */
/*  O : /                                                               */
/************************************************************************/
void resetcolour()
{
    printf("\033[0m;");
}

/************************************************************************/
/*  I : messages to be displayed                                        */
/*  P : displays the message in green                                   */
/*  O : /                                                               */
/************************************************************************/
void print_success(char* msg, ...)
{
    fprintf(stdout, "\033[0;%dm%s\033[0m\n", GREEN, msg);
}

/************************************************************************/
/*  I : messages to be displayed                                        */
/*  P : displays the message in bold red                                */
/*  O : /                                                               */
/************************************************************************/
void print_error(char* msg, ...)
{
    char final_msg[128] = {0};
    va_list arg;

    va_start(arg, msg);
    vsprintf(final_msg, msg, arg);

    fprintf(stderr, "\033[1;%dm%s\033[0m\n", RED, final_msg);

    va_end(arg);

    //strcpy(final_msg, msg);

    //fprintf(stderr, "\033[1;31m%s\033[0m\n", final_msg);
}



