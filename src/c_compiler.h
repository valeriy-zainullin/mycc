#pragma once

#include <stddef.h>
#include <stdint.h>

#if BULDING_MYCC
#include "c_compiler.tokens.h"
extern
#endif

/*
struct tokenizer_location {
	size_t line;
	size_t col;
};
*/
// There aren't any multiline tokens. Comments are discarded anyway.

// Limit file size. Not more than 1GB.
// typedef uint32_t token_offset;

int cc_handle_keyword(char const* text);
int cc_handle_identifier(char const* text);

int cc_handle_integer_constant(char const* text);
int cc_handle_floating_constant(char const* text);
int cc_handle_character_constant(char const* text);
int cc_handle_string_literal(char const* text);
int cc_handle_punctuator(char const* text);

int cc_handle_invalid_token(char const* text);

void cc_notify_token_handled();

// TODO: use bison reentrant tokenizer and parser at some point. Will take some refactoring, but it's a better style.
// This will become a parameter to yylex.
extern struct tokenizer_location tokenizer_current_location;

int yylex();
