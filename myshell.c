/* 
 * Skeleton code for Shell processing
 * This file contains skeleton code for executing commands parsed in main-x.c.
 * Acknowledgement: derived from UCLA CS111
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "cmdline.h"
#include "myshell.h"
#include <sys/types.h>
#include <signal.h>
#include <glob.h>




/** 
 * Reports the creation of a background job in the following format:
 *  [job_id] process_id
 * to stderr.
 */
void report_background_job(int job_id, int process_id);

// Function that handles the glob error
int glob_error(const char *err_path, int err_no){
	printf("(wildcard error)\n");
	return 0;
}

/* command_exec(cmd, pass_pipefd)
 *
 *   Execute the single command specified in the 'cmd' command structure.
 *
 *   The 'pass_pipefd' argument is used for pipes.
 *   On input, '*pass_pipefd' is the file descriptor used to read the
 *   previous command's output.  That is, it's the read end of the previous
 *   pipe.  It equals STDIN_FILENO if there was no previous pipe.
 *   On output, command_exec should set '*pass_pipefd' to the file descriptor
 *   used for reading from THIS command's pipe.
 *   If this command didn't have a pipe -- that is, if cmd->commandop != PIPE
 *   -- then it should set '*pass_pipefd = STDIN_FILENO'.
 *
 *   Returns the process ID of the forked child, or < 0 if some system call
 *   fails.
 *
 *   You must also handle the internal commands "cd" and "exit".
 *   These are special because they must execute in the shell process, rather
 *   than a child.  (Why?)
 *
 *   However, these special commands still have a status!
 *   For example, "cd DIR" should return status 0 if we successfully change
 *   to the DIR directory, and status 1 otherwise.
 *   Thus, "cd /tmp && echo /tmp exists" should print "/tmp exists" to stdout
 *   iff the /tmp directory exists.
 *   Not only this, but redirections should work too!
 *   For example, "cd /tmp > foo" should create an empty file named 'foo';
 *   and "cd /tmp 2> foo" should print any error messages to 'foo'.
 *
 *   How can you return a status, and do redirections, for a command executed
 *   in the parent shell?
 *   Hint: It is easiest if you fork a child ANYWAY!
 *   You should divide functionality between the parent and the child.
 *   Some functions will be executed in each process.
 */
static pid_t
command_exec(command_t *cmd, int *pass_pipefd)
{
	pid_t pid = -1;		// process ID for child
	int pipefd[2];		// file descriptors for this process's pipe
	int status;
	/* EXERCISE: Complete this function!
	 * We've written some of the skeleton for you, but feel free to
	 * change it.
	 */

	// Create a pipe, if this command is the left-hand side of a pipe.
	// Return -1 if the pipe fails.
		
		/* Your code here. */
	   
	 if(cmd->subshell && cmd->argv[0] && !(strcmp(cmd->argv[0],"") == 0)){
	 	goto error;
	 }

	if (cmd->controlop == CMD_PIPE) {	
		if(pipe(pipefd)){
			printf("Error creating pipe\n");
			return -1;
		}
	}


	// Fork the child and execute the command in that child.
	// You will handle all redirections by manipulating file descriptors.
	//
	// This section is fairly long.  It is probably best to implement this
	// part in stages, checking it after each step.  For instance, first
	// implement just the fork and the execute in the child.  This should
	// allow you to execute simple commands like 'ls'.  Then add support
	// for redirections: commands like 'ls > foo' and 'cat < foo'.  Then
	// add parentheses, then pipes, and finally the internal commands
	// 'cd' and 'exit'.
	//
	// In the child, you should:
	//    1. Set up stdout to point to this command's pipe, if necessary.
	//    2. Set up stdin to point to the PREVIOUS command's pipe (that
	//       is, *pass_pipefd), if appropriate.
	//    3. Close some file descriptors.  Hint: Consider the read end
	//       of this process's pipe.
	//    4. Set up redirections.
	//       Hint: For output redirections (stdout and stderr), the 'mode'
	//       argument of open() should be set to 0666.
	//    5. Execute the command.
	//       There are some special cases:
	//       a. Parentheses.  Execute cmd->subshell.  (How?)
	//       b. A null command (no subshell, no arguments).
	//          Exit with status 0.
	//       c. "exit".
	//       d. "cd".
	//
	// In the parent, you should:
	//    1. Close some file descriptors.  Hint: Consider the write end
	//       of this command's pipe, and one other fd as well.
	//    2. Handle the special "exit" and "cd" commands.
	//    3. Set *pass_pipefd as appropriate.
	//
	// "cd" error note:
	// 	- Upon syntax errors: Display the message
	//	  "cd: Syntax error on bad number of arguments"
	// 	- Upon system call errors: Call perror("cd")
	//
	// "cd" Hints:
	//    For the "cd" command, you should change directories AFTER
	//    the fork(), not before it.  Why?
	//    Design some tests with 'bash' that will tell you the answer.
	//    For example, try "cd /tmp ; cd $HOME > foo".  In which directory
	//    does foo appear, /tmp or $HOME?  If you chdir() BEFORE the fork,
	//    in which directory would foo appear, /tmp or $HOME?
	//

	if(!cmd->subshell && strcmp(cmd->argv[0],"exit") == 0){
		// execute exit
		exit(0);
	}

	if(!cmd->subshell && (strcmp(cmd->argv[0],"") == 0))
		return -1;
	
	//printf("Spawning new process\n");

	// Handle reaping of zombie processes automatically
	// struct sigaction sigact;
	
	// sigact.sa_handler = SIG_IGN;

	// sigemptyset(&sigact.sa_mask);

	// sigact.sa_flags = 0;

	// if (sigaction(SIGCHLD, &sigact, 0) == -1) {
	// 	printf("Triggered\n");
	// 	perror(0);
	//   	exit(1);
	// }

	// if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
	//   perror(0);
	//   exit(1);
	// }	

	/* Your code here. */
	pid = fork();
	
	// parent process
	if (pid > 0){
		if(cmd->controlop == CMD_PIPE) {
			*pass_pipefd = pipefd[0];
			close(pipefd[1]);
		}
		if(cmd->argv[0]){
			if(strcmp(cmd->argv[0],"cd") == 0){
				if(!cmd->argv[1]){
					if(chdir(getenv("HOME")) != 0){
						int lsts;
						waitpid(pid, &lsts, WNOHANG);
						perror("cd");
						return -1;
					}
					return 0;
				}
				else if ((cmd->argv[1])[0] == '~'){
					cmd->argv[1] = strcat(getenv("HOME"),(cmd->argv[1])+1);	
				}
				if(chdir(cmd->argv[1]) != 0){
					int lsts;
					waitpid(pid, &lsts, WNOHANG);
					perror("cd");
					return -1;
				}
			}
			else if(strcmp(cmd->argv[0],"jobs") == 0){
				if(jobindex) {
					printf("PID    PPID   STATUS\n");
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
						printf("%d  %d  %s\n",joblist[j]->pid,joblist[j]->ppid,sts);
					}
					int l, m = 0;
					for(l = 0 ;l<k;l++){
						job_free(removelist[l]-m++);
					}
				}
			}
		} 
		return pid;
	}
	// child process
	else if (pid == 0){
		FILE *fp;

		dup2(*pass_pipefd, STDIN_FILENO);

		if(cmd->redirect_filename[0]){
			fp = fopen(cmd->redirect_filename[0], "r");
			dup2(fileno(fp),0);
		}
		if(cmd->redirect_filename[1]){
			fp =fopen(cmd->redirect_filename[1], "w");
			dup2(fileno(fp), 1);
		}
		if(cmd->redirect_filename[2]){
			//close(2);
			fp = fopen(cmd->redirect_filename[2], "w");
			dup2(fileno(fp), 2);
		}

		if(cmd->controlop == CMD_PIPE) {

				dup2(pipefd[1],STDOUT_FILENO);
				close(pipefd[1]);
		}

		if(cmd->subshell){
			//printf("Entered inside: %s\n",(cmd->subshell)->argv[0]);

				status = command_line_exec(cmd->subshell);
				//printf("Status: %d\n",status);
				// Return teh status of the last executed command
				if(status)
					kill(getpid(),SIGKILL);
				//printf("Reached exit error\n");
				exit(status);
		}
		

		// printf("In child process [0]:%s, %s\n",cmd->argv[0], cmd->redirect_filename[1]);
		if(strcmp(cmd->argv[0],"cd") == 0){
			// Exit
			exit(0);
		}
		if(strcmp(cmd->argv[0],"jobs") == 0){
			// Exit
			exit(0);
		}

		//handle wildcard entries
		int z,m=0;
		char *local_cmd[512];
		local_cmd[m++] = cmd->argv[0];
		for(z = 1; z < 512 ;z++){
			//printf("Reached %d\n",z );
			local_cmd[m++] = cmd->argv[z]?cmd->argv[z]:0;
			//printf("Reached %d\n",z );
			if(!cmd->argv[z])
				break;
			// if((strcmp(local_cmd[0],"ls") == 0) && (strstr(cmd->argv[z],"*") != NULL)) {
			if(strstr(cmd->argv[z],"*") != NULL) {	
				m--;
				const char *pattern = cmd->argv[z];
				glob_t results;
				// glob function : glob(const char *pattern, int flags, int (*errfunc) (const char *epath, int eerrno), glob_t *pglob);
	       		if(glob(pattern,0,glob_error, &results) != 0)
	       			exit(1);
	       		unsigned int k;
	       		for (k = 0; k < results.gl_pathc; k++){
	       			//printf("%s\n", results.gl_pathv[k]);
	       			local_cmd[m++] = results.gl_pathv[k];
	       		}
	       		break;
	       		globfree(& results);
			}
		}
		local_cmd[m] = 0;
		
		execvp(local_cmd[0],local_cmd);

		// execvp(cmd->argv[0],cmd->argv);

		return -1;
	}

	error:
		printf("Error\n");
		return -1;
}



/* command_line_exec(cmdlist)
 *
 *   Execute the command list.
 *
 *   Execute each individual command with 'command_exec'.
 *   String commands together depending on the 'cmdlist->controlop' operators.
 *   Returns the exit status of the entire command list, which equals the
 *   exit status of the last completed command.
 *
 *   The operators have the following behavior:
 *
 *      CMD_END, CMD_SEMICOLON
 *                        Wait for command to exit.  Proceed to next command
 *                        regardless of status.
 *      CMD_AND           Wait for command to exit.  Proceed to next command
 *                        only if this command exited with status 0.  Otherwise
 *                        exit the whole command line.
 *      CMD_OR            Wait for command to exit.  Proceed to next command
 *                        only if this command exited with status != 0.
 *                        Otherwise exit the whole command line.
 *      CMD_BACKGROUND, CMD_PIPE
 *                        Do not wait for this command to exit.  Pretend it
 *                        had status 0, for the purpose of returning a value
 *                        from command_line_exec.
 */
int
command_line_exec(command_t *cmdlist)
{
	int cmd_status = 0;	    // status of last command executed
	int pipefd = 0;  // read end of last pipe
	pid_t pid;

	command_t *head = cmdlist;

	while (cmdlist) {
		int wp_status;	    // Hint: use for waitpid's status argument!
				    // Read the manual page for waitpid() to
				    // see how to get the command's exit
				    // status (cmd_status) from this value.

		// TODO: Fill out this function!
		// If an error occurs in command_exec, feel free to abort().

		/* Your code here. */
		if(!cmd_status){
			pid = command_exec(cmdlist, &pipefd);
			//printf("*** PID %d ***\n",pid);
			switch(cmdlist->controlop){
				case CMD_BACKGROUND: wp_status = 0;
									 job_t *job = job_alloc();
									 job->pid = pid;
									 job->ppid = getpid();
									 job->status = RUNNING;
									 job->printstatus = 0;
									 if(jobindex < 99)
									 	joblist[jobindex++]=job;
									 break;
				case CMD_PIPE: wp_status = 0;
								break;
				case CMD_END: if(!pid)
								break;
				default: waitpid(pid, &wp_status, 0);
						//printf("pid %d status %d ifsignalled: %s\n",pid,wp_status,WIFSIGNALED(wp_status) ? "true" : "false");
			}
		}

		if(pid == -1)
			wp_status = 1;

		switch(cmdlist->controlop){
			case CMD_BACKGROUND: break;
			case CMD_PIPE: 	break;
			case CMD_AND: 	cmd_status = wp_status ? 1 : 0;
							break;
			case CMD_OR: 	cmd_status = wp_status ? 0 : 1;
							break;
			case CMD_END: cmd_status = wp_status;
							break;
			case CMD_SEMICOLON:
			default: cmd_status = 0;
		}

		cmdlist = cmdlist->next;
	}
//cmd_status = 0;
done:
	// Free the commant_t structure
	command_free(head);
	return cmd_status;

}


void report_background_job(int job_id, int process_id) {
    fprintf(stderr, "[%d] %d\n", job_id, process_id);
}
