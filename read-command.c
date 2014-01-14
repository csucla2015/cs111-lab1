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
	INVALID,\

} token_class;

/*Get next token. i.e. set of character separated by whitespace. Newline is treated 
 * as a special token and is returned. Comments are ignored and the next token after a comment is returned
 */
char* get_next_token(command_stream_t stream)
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
  
  printf("The next token is %s\n", token);
  return token;
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
		putchar(curr);
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


command_t make_new_command()
{
	command_t com = checked_malloc(sizeof(struct command));
	com->type = SIMPLE_COMMAND;
	com->status = -1;
	char* input = NULL;
	char* output = NULL;
	return com;
}

void prase_simple_command()
{
	char* token = get_next_token(stream);
	command_t c = make_new_command();
	int word_num = 0;

        while(next_token_type(token) == WORD)
        {
                c->u.word[word_num] = token;
		word_num++;
		token = get_next_token(stream);
        }

	if(next_token_type(token) == LEFT_ANGLE)
	{
		c->input = get_next_token(stream); //Add check for word after redirect
	}
	
	if(next_token_type(token) == )

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
  while(stream->next_char != EOF)
  {
	char* token = get_next_token(stream);
	if(isWord(token))
	{
		command_t c = make_new_command();
		c->u.word[0] = token;
		stream->next_command = c;
	}

	
  
  }
  
  return stream;

}




command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
//  error (1, 0, "command reading not yet implemented");
  return s->next_command;
}
