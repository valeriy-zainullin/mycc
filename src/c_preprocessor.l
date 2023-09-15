/* Source of the syntax: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf, page 61, part 6.4, C99. */

/* TODO: if comments are stripped by preprocessor, move comments to preprocessor. */

/* TODO: put locations of grammar rules as done in parser. */

/* yywrap function called at the end of file, so the program could open the next file and continue scanning. */
/* We don't support this for now, but we will have to support compiling multiple files, but into a single exe, right? Not to a single .o file. I think. I should check what GCC implementation does. */
/* For debugging you could use "%option noyywrap debug verbose" as well here. */
%option noyywrap

/* To silence warnings -Werror=unused-function for input and yyunput */
%option noinput nounput

/* Getting rid of -Werror=implicit-function for fileno */
/* More info at https://stackoverflow.com/a/46223160 */
%option never-interactive

%{
	#include "c_preprocessor.redef.h"
%}

/* Track line numbers as well. */
/*%option yylineno 
%{
	#include <assert.h>
	#define YY_USER_ACTION assert(yylineno > 0); tokenizer_current_location.line = (size_t) yylineno;
	#include "tokenizer.h"
	struct tokenizer_location tokenizer_current_location = {0};
	
	#include <string.h>
	void accnt_mltln_comment_in_location(char const* text);
%}*/
// Some time in the future I'll implement it. Too much work at the start.
//   Implement it like gcc does: emit a #line ... directive. And extend c compiler tokenizer and parser to accept and account them.

/*
pp-number:
	digit
	. digit
	pp-number digit
	pp-number identifier-nondigit
	pp-number e sign
	pp-number E sign
	pp-number p sign
	pp-number P sign
	pp-number .
*/

/*
preprocessing-token:
	header-name
	identifier
	// pp-number          // Not supported for now.
	// character-constant // Not supported for now.
	// string-literal     // Not supported for now.
	punctuator
	each non-white-space character that cannot be one of the above
*/

/*
preprocessing-file:
	group_opt
*/

/*
group:
	group-part
	group group-part
*/

/*
group-part:
	if-section
	control-line
	text-line
	# non-directive
*/

/*
if-section:
	if-group elif-groups_opt else-group_opt endif-line
*/

/*
if-group:
	# if constant-expression new-line group_opt
	# ifdef identifier new-line group_opt
	# ifndef identifier new-line group_opt
*/

/*
elif-groups:
	elif-group
	elif-groups elif-group
*/

/*
elif-group:
	# elif constant-expression new-line group_opt
*/

else-group:
	# else new-line group_opt
endif-line:
	# endif new-line

control-line:
	# include pp-tokens new-line
	# define identifier replacement-list new-line
	# define identifier lparen identifier-list_opt ) replacement-list new-line
	# define identifier lparen ... ) replacement-list new-line
	# define identifier lparen identifier-list , ... ) replacement-list new-line
	# undef identifier new-line
	# line pp-tokens new-line
	# error pp-tokensopt new-line
	# pragma pp-tokensopt new-line
	# new-line

text-line:
	pp-tokens_opt new-line

non-directive:
	pp-tokens new-line

lparen:
	a ( character not immediately preceded by white-space

replacement-list:
	pp-tokens_opt

pp-tokens:
	preprocessing-token
	pp-tokens preprocessing-token

new-line:
	the new-line character
