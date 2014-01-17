// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"
#include <error.h>
#include <stdio.h>
#include <string.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

struct command_stream {
	int (*get_next_byte)(void* arg);
	void *get_next_byte_argument;	
	char* current_token;
	char next_char;
        int line_number;
	command_t next_command;
};

/* Enumerated type to define type of token */
typedef enum {
	WORD,\
	SEMICOLON,\
	PIPE,\
	AND,\
	OR,\
	LEFT_ANGLE,\
	RIGHT_ANGLE,\
	LEFT_ROUND,\
	RIGHT_ROUND,\
	NEWLINE,\
	WHITESPACE,\
	END_OF_FILE,\
	INVALID,\

} token_class;

/*Get next token. i.e. set of character separated by whitespace. Newline is treated 
 * as a special token and is returned. Comments are ignored and the next token after a comment is returned
 */
void get_next_token(command_stream_t stream)
{
//TODO refactor alloc later
  char* token = checked_malloc(10 * sizeof(char));
  int token_size = 0;
  int max_token_size = 10;
  char next_char =' ';
  if(stream->next_char == ~(3)+1)
	next_char =  (*stream->get_next_byte)(stream->get_next_byte_argument);
  else next_char = stream->next_char;


   while(next_char == ' ' || next_char == '\t')
	{	
		next_char = (*stream->get_next_byte)(stream->get_next_byte_argument);
	}

    if(next_char == EOF)
    {
        char * a = checked_malloc(sizeof(char));
        a[0] = EOF;
        stream->current_token = a;
	return;
    }
  
   if(next_char == '\n' || next_char == '(' || next_char == ')' || next_char == ';' || next_char == '<' || next_char == '>')
  {
	token[0] = next_char;
        
	if(next_char == '\n')
	stream->line_number++;

	next_char = (*stream->get_next_byte)(stream->get_next_byte_argument);
	goto ret;	
  }
  
  if(next_char == '&')
  {
	next_char = (*stream->get_next_byte)(stream->get_next_byte_argument);
	if(next_char == '&'){
	token[0] = '&';
	token[1] = '&';
	token_size++;
	next_char = (*stream->get_next_byte)(stream->get_next_byte_argument);
         }
	goto ret;
 }

   if(next_char == '|')
  {
        next_char = (*stream->get_next_byte)(stream->get_next_byte_argument);
        
	if(next_char == '|'){
        token[0] = '|';
        token[1] = '|';
	token_size++;
        next_char = (*stream->get_next_byte)(stream->get_next_byte_argument);	
	}

	else token[0] = '|';
        goto ret;
 }

  if(next_char == '#')
  {
	while(next_char != '\n')
		next_char = (*stream->get_next_byte)(stream->get_next_byte_argument);
  
  next_char = (*stream->get_next_byte)(stream->get_next_byte_argument); // Skip to next char after comment newline
  }
 
  
  while(next_char != EOF && next_char != ' ' && next_char != '\n' && next_char != '\t' && next_char != ';' && next_char != ')' && next_char != '>' && next_char != '<' && next_char != '|' && next_char != '&')
	{	
		if(token_size == max_token_size){
			max_token_size += 10;
			token = checked_realloc(token,max_token_size*sizeof(char));
					
		}
		token[token_size] = next_char;
		next_char = (*stream->get_next_byte)(stream->get_next_byte_argument);	
		token_size++;
		
	}
  

  ret:
  token[token_size + 1] = 0;
  stream->next_char = next_char;
  stream->current_token = token;
  
  ////printf("The next token is %s\n", token);
  //return token;
}  


/*
 * Check whether a token contains valid word characters
 */
int isWord(char* token)
{
	int index = 0;
	while(token[index] != '\0')
	{	
		char curr = token[index];
		if((curr >= '0' && curr <= '9') || (curr >= 'a' && curr <= 'z') || (curr >= 'A' && curr <='Z') || curr == '!' || curr == '%' || curr == '+' || curr == ',' || curr == '-' || curr == '.' || curr == '/' || curr == ':' || curr == '@' || curr == '^' || curr == '_')
	;	

		else return 1;
		
		index++;
	}
		return 0;


}

/*
 * Get next token type
 */
token_class next_token_type(char* token)
{
	token_class type;

	if(token[0] == EOF)
	{
		type = END_OF_FILE;
		return type;
	}

	if(isWord(token) == 0)
	{
		type = WORD;
		return type;
	}

	if(strcmp(token,"\t") == 0 || strcmp(token," ") == 0)
	{
		type = WHITESPACE;
		return type;
	}

	if(strcmp(token,"\n") == 0)
	{
		type = NEWLINE;
		return type;
	}

	if(strcmp(token,";") == 0)
	{
		type = SEMICOLON;
		return type;
	}

	if(strcmp(token,"|") == 0)
	{
		type = PIPE;
		return type;
	}

	if(strcmp(token,"(") == 0)
	{
		type = LEFT_ROUND;
		return type;
	}

	if(strcmp(token,")") == 0)
	{
		type = RIGHT_ROUND;
		return type;
	}

	if(strcmp(token,"<") == 0)
	{
		type = LEFT_ANGLE;
		return type;
	}

	if(strcmp(token,">") == 0)
	{
		type = RIGHT_ANGLE;
		return type;
	}

	if(strcmp(token,"&&") == 0)
	{
		type = AND;
		return type;
	}

	if(strcmp(token, "||") == 0)
	{ 
		type = OR;
		return type;
	}
	
	type = INVALID;
	return type;
}

void skip_newline(command_stream_t stream)
{
	//if(stream->current_token) get_next_token(stream);	
	while(next_token_type(stream->current_token) == NEWLINE)
	{
		get_next_token(stream);

	}
	
	
}

command_t make_new_command(token_class type)
{
	command_t com = checked_malloc(sizeof(struct command));
	com->type = type;
	com->status = -1;
	char* input = NULL;
	char* output = NULL;
	return com;
}

command_t parse_simple_command(command_stream_t stream)
{

	skip_newline(stream);

	if(next_token_type(stream->current_token) == END_OF_FILE)
		return NULL;


	command_t c = make_new_command(SIMPLE_COMMAND);
	int word_num = 1;
	c->u.word = checked_malloc(sizeof(char *));
	int flag =0;
        while(next_token_type(stream->current_token) == WORD)
        {
	////printf("here\n"); 
                c->u.word[word_num-1] = stream->current_token;
		word_num++;
		c->u.word = checked_realloc(c->u.word,word_num*sizeof(char *));
		get_next_token(stream); 
        	flag = 1;
	}
	
	if(flag == 1) c->u.word[word_num] = NULL;
        else error(1,0,"%d: Incorrect syntax.", stream->line_number);	
	
	if(next_token_type(stream->current_token) == LEFT_ANGLE)
	{
		get_next_token(stream);
		if(next_token_type(stream->current_token) == WORD)
		{
			c->input = stream->current_token;
			get_next_token(stream);
		} 

		else error(1,0,"%d: Unexpected token after \'<\'",stream->line_number);
	}

	
	if(next_token_type(stream->current_token) == RIGHT_ANGLE)
	{
		get_next_token(stream);
                if(next_token_type(stream->current_token) == WORD)
                {
                        c->output = stream->current_token; //Get next char
			get_next_token(stream);
                }

                else error(1,0,"%d: Unexpected token after \'>\'",stream->line_number);
        }
	
	
	//printf("%s is the last token\n",stream->current_token);
	return c;
} 

	
command_t parse_pipe_command(command_stream_t stream)
{
	command_t left = parse_simple_command(stream);
	command_t right = NULL;
	command_t pipe = NULL;
	while(next_token_type(stream->current_token) == PIPE)
	{
		pipe = make_new_command(PIPE_COMMAND);
		get_next_token(stream);
		right = parse_simple_command(stream);
		pipe->u.command[0] = left;
		pipe->u.command[1] = right;
		left = pipe;	
	}

	return left;
		
	

}	

command_t parse_and_or_command(command_stream_t stream)
{
	command_t left = parse_pipe_command(stream);
	command_t right = NULL;
	while(next_token_type(stream->current_token) == AND || next_token_type(stream->current_token) == OR)
	{
		if(next_token_type(stream->current_token) == AND)
		{
			command_t and = make_new_command(AND_COMMAND);
			get_next_token(stream);
			right = parse_pipe_command(stream);
			and->u.command[0] = left;
			and->u.command[1] = right;
			left = and;
		}

		if(next_token_type(stream->current_token) == OR)
		{
			command_t or = make_new_command(OR_COMMAND);
                	get_next_token(stream);
                	right = parse_pipe_command(stream);
                	or->u.command[0] = left;
                	or->u.command[1] = right;
                	left = or;
		}

		 
	}
	//get_next_token(stream);
	return left;				

}


command_t parse_complete_command(command_stream_t stream)
{
	command_t left = parse_and_or_command(stream);
	command_t right = NULL;
	while(next_token_type(stream->current_token) == SEMICOLON)// || next_token_type(stream->current_token) == NEWLINE)
	{
		command_t sequence = make_new_command(SEQUENCE_COMMAND);
		get_next_token(stream);
		right = parse_and_or_command(stream);
		if(right != NULL) {
		sequence->u.command[0] = left;
		sequence->u.command[1] = right;
		left = sequence;
	} else break;
	}
	//get_next_token(stream);
	return left;
}

command_t parse_subshell_command(command_stream_t stream)
{
	command_t subshell = NULL;
	//printf("%s\n",stream->current_token);
	if(next_token_type(stream->current_token) == LEFT_ROUND)
	{//printf("Left\n");
		subshell = make_new_command(SUBSHELL_COMMAND);
		get_next_token(stream);
		//printf("%s\n",stream->current_token);	
		subshell->u.subshell_command = parse_complete_command(stream);
		//printf("%s\n",stream->current_token);
	
        }
	//printf(stream->current_token);
	if(next_token_type(stream->current_token) == RIGHT_ROUND)
	{//printf("Right");
		get_next_token(stream);
		
	

	//else error(1,0,"%d: No closing parenthesis found", stream->line_number);

	if(next_token_type(stream->current_token) == LEFT_ANGLE)
        {
                get_next_token(stream);
                if(next_token_type(stream->current_token) == WORD)
                {
                        subshell->input = stream->current_token;
                        get_next_token(stream);
                }

                else error(1,0,"%d: Unexpected token after \'<\'",stream->line_number);
        }


        if(next_token_type(stream->current_token) == RIGHT_ANGLE)
        {
                get_next_token(stream);
                if(next_token_type(stream->current_token) == WORD)
                {
                        subshell->output = stream->current_token; //Get next char
                        get_next_token(stream);
                }

                else error(1,0,"%d: Unexpected token after \'>\'",stream->line_number);
        }
	
	
	
	
	return subshell;


	}
	return parse_complete_command(stream);

	//return NULL;
	
}

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  command_stream_t stream = checked_malloc(sizeof (struct command_stream) );
  stream->get_next_byte = get_next_byte; stream->next_char = ~(3)+1;
  stream->get_next_byte_argument = get_next_byte_argument;
  stream->line_number = 1; 
  get_next_token(stream); 
  return stream;

}




command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");
  return parse_subshell_command(s);
}
