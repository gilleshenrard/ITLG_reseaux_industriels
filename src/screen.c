/*
** screen.c
** Library regrouping screen-based functions
** ------------------------------------------
** Made by Gilles Henrard
** Last modified : 01/11/2019
*/
#include "screen.h"

/************************************************************************/
/*  I : buffer for the final message to display                         */
/*      message to display                                              */
/*      additional parameters of the message                            */
/*  P : formats the final message to be displayed                       */
/*  O : /                                                               */
/************************************************************************/
void format_output(char* final_msg, char* msg, va_list* arg)
{
    vsprintf(final_msg, msg, *arg);
}

/************************************************************************/
/*  I : messages to be displayed                                        */
/*  P : displays the message in green                                   */
/*  O : /                                                               */
/************************************************************************/
void print_success(char* msg, ...)
{
    char final_msg[SZLINE] = {0};
    va_list arg;

    va_start(arg, msg);

    format_output(final_msg, msg, &arg);
    fprintf(stdout, "\033[0;%dm%s\033[0m\n", GREEN, final_msg);

    va_end(arg);
}

/************************************************************************/
/*  I : messages to be displayed                                        */
/*  P : displays the message in bold red                                */
/*  O : /                                                               */
/************************************************************************/
void print_error(char* msg, ...)
{
    char final_msg[SZLINE] = {0};
    va_list arg;

    va_start(arg, msg);

    format_output(final_msg, msg, &arg);
    fprintf(stderr, "\033[1;%dm%s\033[0m\n", RED, final_msg);

    va_end(arg);
}



