/* 
 * Skeleton code for Lab 1 - Shell processing
 * This file contains skeleton code for executing commands parsed in cmdline.c.
  * Acknowledgement: derived from UCLA CS111
 */

/*
Author: Sumukh Hallymysore Ravindra
Date: 01/23/2016
Project: OS EC695 - Assignment 1
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "cmdline.h"
#include "myshell.h"

/* EXERCISE: Make sure you free memory used by the command_t structures
 * when it is no longer needed.
 * EXERCISE:  Make sure you clean up ALL zombie processes, including those
 * that ran in the background!
 * There are several ways to do this, and several places to put the code.
 * Check out the SIGCHLD signal and the waitpid() system call for hints.
 * HINT:
 *   You DO NOT need to clean up zombies as soon as they are created.
 *   It is OK to clean up zombies as little as once per command.
 *   That is, it is OK if a zombie hangs around until the next time the user
 *   types a command.  But it is NOT OK for zombies to hang around forever.
 */

/* 
 * Main function for shell.
 */
int
main(int argc, char *argv[])
{
	int quiet = 0;
	char input[BUFSIZ];
	int r = 0;


	// Check for '-q' option: be quiet -- print no prompts
	if (argc > 1 && strcmp(argv[1], "-q") == 0)
		quiet = 1;

	while (!feof(stdin)) {
		parsestate_t parsestate;
		command_t *cmdlist;
		// Print the prompt
		if (!quiet) {
			printf("ECE695 prog1$ ");
			//Write the buffered unwritten string into the terminal
			fflush(stdout);
		}

		// Read a string, checking for error or EOF
		if (fgets(input, BUFSIZ, stdin) == NULL) {
			if (ferror(stdin))
				// This function prints a description of the
				// error, preceded by 'ECE695 prog1 Error: '.
				perror("ECE695 prog1 Error: ");
			break;
		}

		//Look for completed jobs
		if(jobindex){
				int j, k = 0; 
				int stspid;
				for(j = 0 ; j<jobindex;j++){
					char *sts;
					// printf("Current Status: %d\n",waitpid(joblist[j]->pid,&stspid,WNOHANG));
					joblist[j]->status = waitpid(joblist[j]->pid,&stspid,WNOHANG) ? DONE : RUNNING ;
					switch(joblist[j]->status){
						default: printf("Error Setting value\n");
								break;
						case RUNNING: sts = "RUNNING";
										break;
						case DONE: sts = "DONE";
										joblist[j]->printstatus = 1;
										break;
						case KILLED: sts = "KILLED";
										break;
					}
				}
		}
		
		
		// TODO: invoke some function(s) in cmdline.c for parsing the read string.

		//initialize the parsestate_t stucture to the beginning of the input command line
		parse_init(&parsestate,input);
		cmdlist = command_line_parse(&parsestate,0);

		if (!cmdlist) {
			printf("Syntax error\n");
			continue;
		}

		// print the command list
		if (!quiet) {
			// TODO: invoke some function(s) in cmdline.c for printing out the command for debugging.
			// command_print(cmdlist, 4);
			// // why do we need to do this?
			// fflush(stdout);
		}

		// and run it!
		if (cmdlist)
			command_line_exec(cmdlist);


		//Look for completed jobs
		if(jobindex){
			//printf("PID    PPID   STATUS\n");
			int removelist[100];
				int j, k = 0; 
				for(j = 0 ; j<jobindex;j++){
					char *sts;
					int stspid;
					// printf("Current Status: %d\n",waitpid(joblist[j]->pid,&stspid,WNOHANG));
					joblist[j]->status = waitpid(joblist[j]->pid,&stspid,WNOHANG) ? DONE : RUNNING ;
					switch(joblist[j]->status){
						default: printf("Error Setting value\n");
								break;
						case RUNNING: sts = "RUNNING";
										break;
						case DONE: sts = "DONE";
										joblist[j]->printstatus = 1;
										removelist[k++] = j;
										break;
						case KILLED: sts = "KILLED";
										break;
					}
					// print the status of background jobs
					if((strcmp(sts,"DONE") == 0) && !quiet)
						printf("Done : %d\n",joblist[j]->pid);
				}
				int l, m = 0;
				for(l = 0 ;l<k;l++){
					job_free(removelist[l]-m++);
				}

		}
				
	}

	return 0;
}
