// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <sys/types.h>
#include <error.h>
#include <unistd.h>
#include <sys/wait.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
command_status (command_t c)
{
  return c->status;
}

void execute_simple_command(command_t command)
{
	pid_t pid;

	if((pid = fork()) < 0)
		error(1,0,"Could not create new process");

	if (pid == 0) {
		if(execvp(command->u.word[0], command->u.word) < 0)
	 		error(1,0,"Command execution failed");

		}

	else waitpid(pid,&command->status,0);

}	


void
execute_command (command_t c, int time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  //error (1, 0, "command execution not yet implemented");
 
	execute_simple_command(c);
}
