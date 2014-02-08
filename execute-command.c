// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <sys/types.h>
#include <error.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
command_status (command_t c)
{
  return c->status;
}

void execute_wrapper(command_t c);

void execute_simple_command(command_t command)
{
        pid_t pid;

        if((pid = fork()) < 0)
                error(1,0,"Could not create new process");

        if (pid == 0) {

                if(command->input != NULL){
                        int fd0 = open(command->input,O_RDONLY,0644); //Open Input File
                        if(fd0 < 0)                                            
                                error(1,0,"Input file does not exist");
                        dup2(fd0,0);  //Copy File descriptor to STDIN
                        close(fd0);
                }


                if(command->output != NULL){
                        int fd1 = open(command->output,O_WRONLY | O_CREAT | O_TRUNC,0644); //Open Output File
                        if(fd1 < 0)
                                error(1,0,"Could not write to output file");
                                dup2(fd1,1);  //Copy File descriptor to STDOUT
                                close(fd1);
                }


                if(execvp(command->u.word[0], command->u.word) < 0)
                         error(1,0,"Command execution failed");

                }

        else waitpid(pid,&command->status,0);
	
	if(command->status == -1)
		command->status = 1;
	else command->status = 0;
        
        

}        


void execute_and_or_command(command_t command)
{


        if(command->type == AND_COMMAND){
                execute_wrapper(command->u.command[0]);
                command->status = command->u.command[0]->status;
                if(command_status(command->u.command[0]) == 0){
                        execute_wrapper(command->u.command[1]);
                        command->status = command->u.command[1]->status;
                }
                else
                		command->status = 1;

        }

        if(command->type == OR_COMMAND){
                execute_wrapper(command->u.command[0]);
                command->status = command->u.command[0]->status;
                //printf("%d\n",command->status);
                if(command_status(command->u.command[0]) > 0){
                        execute_wrapper(command->u.command[1]);
                        command->status = command->u.command[1]->status;
                }
                else
                	command->status = 0;
        }        
        
}        

void execute_pipe_command(command_t command)
{
        int pipe_array[2];
        
        if(pipe(pipe_array) == -1)
                error(1,0,"Error creating pipe");

        pid_t pid = fork();
        if(pid == 0)
        {
                dup2(pipe_array[1],1);
                close(pipe_array[0]);        
                //execvp(command->u.command[0]->u.word[0],command->u.command[0]->u.word);
                execute_wrapper(command->u.command[0]);
        }

        else if(pid > 0){
                
                        dup2(pipe_array[0],0);
                        close(pipe_array[1]);
                        //execvp(command->u.command[1]->u.word[0],command->u.command[1]->u.word);
                        execute_wrapper(command->u.command[1]);
                
                 //waitpid(pid,&command->u.command[1]->status,0);
            }

        else if(pid == -1)
                error(1,0,"Error executing pipe command");

        waitpid(pid,&command->status,0);
	
	if(command->status < 0) command->status = 1;
	else command->status = 0;
	

        
 }


void execute_subshell_command(command_t command)
{

pid_t pid;
                                                                                                      
//if((pid = fork()) < 0)
  //      error(1,0,"Could not create new process");
                                                                                                      
//if (pid == 0) {
                                                                                                      
        if(command->input != NULL){
                int fd0 = open(command->input,O_RDONLY,0644); //Open Input File
                if(fd0 < 0)                                            
                        error(1,0,"Input file does not exist");
                dup2(fd0,0);  //Copy File descriptor to STDIN
                close(fd0);
        }
                                                                                                      
                                                                                                      
        if(command->output != NULL){
                int fd1 = open(command->output,O_WRONLY | O_CREAT | O_TRUNC,0644); //Open Output File
                if(fd1 < 0)
                        error(1,0,"Could not write to output file");
                        dup2(fd1,1);  //Copy File descriptor to STDOUT
                        close(fd1);
        }
                                                                                                      
                       	                                                                               
        execute_wrapper(command->u.subshell_command);
        command->status = command->u.subshell_command->status;
                 //error(1,0,"Command execution failed");

//}        
                                                                                                      
//else waitpid(pid,&command->status,0);

                
}

void execute_wrapper(command_t command);
void execute_sequence_command(command_t command)
{
        execute_wrapper(command->u.command[0]);
        execute_wrapper(command->u.command[1]);
	command->status = command->u.command[1]->status; 
}


void execute_wrapper(command_t c)
{
         switch(c->type){
                                          
               case (SIMPLE_COMMAND):
               execute_simple_command(c);
               break;
               case (AND_COMMAND):
               case (OR_COMMAND):
               execute_and_or_command(c);
               break;
               case (SEQUENCE_COMMAND):
                execute_sequence_command(c);
                break;
               case (PIPE_COMMAND):
               execute_pipe_command(c);
               break;
               case (SUBSHELL_COMMAND):
                execute_subshell_command(c);
                break;
        }
}


void
execute_command (command_t c, int time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  //error (1, 0, "command execution not yet implemented");

  execute_wrapper(c);

}
