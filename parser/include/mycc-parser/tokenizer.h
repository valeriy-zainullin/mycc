#pragma once

#include <string>

namespace tokenizer {
	struct Location {
		std::string filename;
		size_t line;
		size_t col;
	};
	
	// There aren't any multiline tokens. Comments are discarded anyway.

	// Limit file size. Not more than 1GB.
	// typedef uint32_t token_offset;

	int handle_keyword(char const* text);
	int handle_identifier(char const* text);
	
	int handle_integer_constant(char const* text);
	int handle_floating_constant(char const* text);
	int handle_character_constant(char const* text);
	int handle_string_literal(char const* text);
	int handle_punctuator(char const* text);
	
	int handle_invalid_token(char const* text);
	
	void notify_token_handled();

	// TODO: use bison reentrant tokenizer and parser at some point. Will take some refactoring, but it's a better style.
	// This will become a parameter to yylex.
	extern Location current_location;
}

#include <cstdio>

extern FILE* yyin;
extern int   yylex(void);
