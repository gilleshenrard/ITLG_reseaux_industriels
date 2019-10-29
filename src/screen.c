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
/*  I : text to be displayed                                            */
/*  P : displays the message in green                                   */
/*  O : /                                                               */
/************************************************************************/
void print_success(char* msg)
{
    fprintf(stdout, "\033[0;32m%s\033[0m\n", msg);
}

/************************************************************************/
/*  I : text to be displayed                                            */
/*      flag to indicate if errno has been set                          */
/*  P : displays the message in bold red                                */
/*  O : /                                                               */
/************************************************************************/
void print_error(char* msg, int errno_set)
{
    char final_msg[128] = {0};

    if(errno_set)
        sprintf(final_msg, "%s: %s", msg, strerror(errno));
    else
        strcpy(final_msg, msg);

    fprintf(stderr, "\033[1;31m%s\033[0m\n", final_msg);
}
