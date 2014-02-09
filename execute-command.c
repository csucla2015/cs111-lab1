// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include "alloc.h"
#include <sys/types.h>
#include <error.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>  
#include <sys/wait.h> 
#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <error.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/stat.h>		
#include <fcntl.h>
#include <stdio.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */
typedef struct node* node_t;
typedef struct child_node* child_node_t;
typedef struct command_node* command_node_t;
struct node
{
  char* word;
  struct node* next;
};

// Has a pointer to dependent
struct child_node
{
  struct command_node* dependent;
  struct child_node* next;

};

//Nodes describing top level commands and their dependencies
struct command_node
{
  struct command* c;
  struct node* inputs;
  struct node* outputs;
  int dependencies;
  struct child_node* dependents;
  int pid;
  struct command_node* next;
};


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
pid_t pid1 , pid2;

if(pipe(pipe_array) == -1) error(1, 0, "pipe creation failed");

pid1 = fork();
if(pid1 > 0)
{
        pid2 = fork();
        if(pid2 >0 )
        {
                close(pipe_array[0]);
                close(pipe_array[1]);

                pid_t temp = waitpid(-1,&command->status,0);
                if(temp == pid1)
                {
                        waitpid(pid2,&command->status,0);
                        return;
                }

                if(temp == pid2)
                {
                        waitpid(pid1,&command->status,0);
                        return;
                }

        }



        else if(pid2 == 0)
        {

        close(pipe_array[0]);
        dup2(pipe_array[1],1);
        execute_wrapper(command->u.command[0]);
        exit(command->u.command[0]->status);
        }

}

else if (pid1 == 0)
{
        close(pipe_array[1]);
        dup2(pipe_array[0],0);
        execute_wrapper(command->u.command[1]);
        exit(command->u.command[1]->status);
}

else error(1,0,"Fork failure");

        
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

//All the dependencies are added to the node and all the dependencies of the subcommand are added to the node.
void add_dependencies(command_node_t node, command_t command)
{
	if(command->input != 0)
	{
		if(node->inputs == NULL)
		{
			int size = sizeof(node_t);
			node_t head = checked_malloc(size);
			head->word = command->input;
			head->next = NULL;
		}	
		else
		{
			//Adding word to the list
			while(strcmp(node->inputs->word, command->input) == 0)
			{	
			 if(node->inputs->next == NULL)
			{
				node->inputs->next = checked_malloc(sizeof(node_t));
				node->inputs->next->word = command->input; // Word getting added
				node->inputs->next->next = NULL;
			}
			else
				node->inputs->next;
			}
		}	
	}


	if(command->output != 0)
	{
		if(node->outputs == NULL)
		{
			int size = sizeof(node_t);
			node_t head = checked_malloc(size);
			head->word = command->output;
			head->next = NULL;
			node->outputs =  head;
		}	
		else
		{
			//Adding word to the list
			while(strcmp(node->outputs->word, command->output) == 0)
			{	
				 if(node->outputs->next == NULL)
				{
					node->outputs->next = checked_malloc(sizeof(node_t));
					node->outputs->next->word = command->output;; // Word getting added
					node->outputs->next->next = NULL;
				}
				else
					node->outputs->next;
			}
		}
	}

	int temp = 0;
	if(command->type == AND_COMMAND || command->type == OR_COMMAND || command->type == SEQUENCE_COMMAND || command->type == PIPE_COMMAND)
	{
		add_dependencies(node, command->u.command[0]); //Above mentioned commands could have input and out, so have to 
		//to add dependencies both for input and output. 
		add_dependencies(node, command->u.command[1]);
	}	
	if(command->type == SUBSHELL_COMMAND )
		add_dependencies(node, command->u.subshell_command);
	if(command->type == SIMPLE_COMMAND)
	{	
				temp = 1;
			while(command->u.word[temp] != NULL)
			{
				if(node->inputs == NULL)
				{	
						int size = sizeof(node_t);
						node_t head = checked_malloc(size);
						head->word = command->u.word[temp];
						head->next = NULL;
						node->inputs = head;
				}		
				else
				{
					//Adding word to the list
					while(strcmp(node->inputs->word, command->u.word[temp]) == 0)
					{	
					 if(node->inputs->next == NULL)
					{
						node->inputs->next = checked_malloc(sizeof(node_t));
						node->inputs->next->word = command->u.word[temp]; // Word getting added
						node->inputs->next->next = NULL;
					}
					else
						node->inputs->next;
				}
				}
					

				temp++;
			}
	}
}

 int helper(command_node_t previous, node_t outputs, node_t inputs, command_node_t next_dependent )
{
  node_t current_output = outputs;
  
  while(current_output != NULL)
  {
    node_t current_input = inputs;
    for(;;)
    {
    	if(current_input == NULL)
    		break;
      if(strcmp(current_input->word, current_output->word) == 0)
      {
      	//increamenting dependencies and making the waiting list know
          next_dependent->dependencies += 1;

          child_node_t depend_list = previous->dependents;
		  child_node_t last_node = depend_list;
		  while(depend_list != NULL)
		  {
		    last_node = depend_list;
		    depend_list = depend_list->next;
		  }
		  child_node_t new_node = checked_malloc(sizeof(struct child_node));
		  new_node->dependent = next_dependent;
		  new_node->next = NULL;
			if(last_node == NULL)
				previous->dependents = new_node;
			else
				last_node->next = new_node;


          return -1;
      }
      current_input = current_input->next;
    }
    current_output = current_output->next;
  }
  return 1;
}

command_t
execute_time_travel (command_stream_t com)
{
  command_node_t dep_head = NULL;

  command_t final_command = NULL;
  command_t command;
  for(;;)
  {
  	command = read_command_stream (com);

  	if(!command)
  	break;
    command_node_t new_node = checked_malloc(sizeof(struct command_node));
    new_node->c = command;
    new_node->inputs = NULL;
    new_node->outputs = NULL;
    new_node->dependencies = 0;
    new_node->dependents = NULL;
    new_node->pid = -1;
    
   

    add_dependencies(new_node, command);  

    // For all on the list, walk through all even if there one is found
    command_node_t last_node = dep_head;
    command_node_t current_node = dep_head;
    for(;;)
    {
    	if(current_node == NULL)	
    		break;
    
      //If dependency is found
      helper(current_node, new_node->outputs, current_node->inputs, new_node);
      helper(current_node, current_node->outputs, new_node->inputs, new_node);
      
      last_node = current_node;
      current_node = current_node->next;
    }

    // Add the node to the list of executing and waiting commands
    if( last_node == NULL)
      dep_head = new_node;
    else
      last_node->next = new_node;

    final_command = command;
  }

  // While there's someone on the waiting list
  for(;;)
  {
  	if(dep_head == NULL)
  		break; 	
    command_node_t current_node = dep_head;
    // For everyone on the list
    for(;;)
    {
    	if(current_node==NULL)
    		break;
      // If they're not waiting on anyone
      if(current_node->dependencies == 0 && current_node->pid < 1)
      {
        //fork and execute, indicate its pid
        int pid = fork();
        if(pid == -1)
						error(1, 0, "Could not fork");
				else if( pid == 0)
        {
						execute_command(current_node->c,0);
						exit(0);
        }
				else if( pid > 0)
        {
						current_node->pid = pid;
        }
        else{int temp=0;}
      }
        
      current_node = current_node->next;
    }
    
    int status;
    // Wait for someone to finish
    pid_t pid1 = waitpid(-1, &status, 0);
    
    // Use pid to determine who finished and remove them
    command_node_t previous_node = NULL;
    command_node_t last_node = dep_head;
    for(;;)
    {
    	if(last_node == NULL)
    		break;
      // If they're not waiting on anyone
      if(last_node->pid == pid1)
      {
        child_node_t current_dependency = last_node->dependents;
        // for all on the list of dependents
        while(current_dependency != NULL)
        {
          command_node_t temp_node = current_dependency->dependent;
          // free that dependent (reduce dependencies)
          temp_node->dependencies -= 1;
          
          current_dependency = current_dependency->next;
        }
          
        // remove from the list
        if(previous_node == NULL)
          dep_head = last_node->next;
        else
          previous_node->next = last_node->next;
        break;
      }
      
      previous_node = last_node;
      last_node = last_node->next;
    }
    
  }

  return final_command;
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
