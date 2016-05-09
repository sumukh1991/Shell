#ifndef PROG1_MYSHELL_H
#define PROG1_MYSHELL_H

/* 
 * Header file Shell processing
 * This file contains the definitions required for executing commands
 * parsed in main-b.c.
 * Credits: UCLA CS111
 */

#include "cmdline.h"

// global list to store the backgroung running jobs
extern job_t *joblist[100];

extern int jobindex;

/* Execute the command list. */
int command_line_exec(command_t *);

#endif
