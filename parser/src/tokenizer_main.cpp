#include "tokenizer.h"

#include <cassert>
#include <cstdio>
#include <string_view>

using namespace std::literals;

// TODO: move to a separate header if needed later in other files.
#define UNREACHABLE assert(false); __builtin_unreachable();

static void print_escaped_string(char const* string) {
	for (; *string != '\0'; ++string) {
		if (*string == '"') {
			printf("\\");
		}
		printf("%c", *string);
	}
}

static void print_token(char const* label, char const* value, tokenizer::Location const& location) {
	printf("<%s:\"", label);
	print_escaped_string(value);
	printf("\":%zu,%zu> ", location.line, location.col);
}

int tokenizer::handle_keyword(char const* text) {
	print_token("keyword", text, current_location);
	notify_token_handled();
	return 1;
}

int tokenizer::handle_punctuator(char const* text) {
	print_token("punctuator", text, current_location);
	notify_token_handled();
	return 1;
}

int tokenizer::handle_identifier(char const* text) {
	print_token("identifier", text, current_location);
	notify_token_handled();
	return 1;
}

int tokenizer::handle_integer_constant(char const* text) {
	print_token("integer-constant", text, current_location);
	notify_token_handled();
	return 1;
}

int tokenizer::handle_floating_constant(char const* text) {
	print_token("floating-constant", text, current_location);
	notify_token_handled();
	return 1;
}

int tokenizer::handle_character_constant(char const* text) {
	print_token("character-constant", text, current_location);
	notify_token_handled();
	return 1;
}

int tokenizer::handle_string_literal(char const* text) {
	print_token("string-literal", text, current_location);
	notify_token_handled();
	return 1;
}

int tokenizer::handle_invalid_token(char const* text) {
	printf("*yytext = %c\n", *text);
	/* Trigger an error. Unknown token. But returning -1 here is a temporary solution. */
	/* Better to do some recovery. */
	printf("Unknown token.\n");
	// TODO: check if it is needed to be done.
	// tokenizer_notify_token_handled();
	return -1;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		return 1;
	}
	
	if (argv[1] == "-"sv) {
		yyin = stdin;
		
		tokenizer::current_location.filename = std::string("stdin");
	} else {
		yyin = fopen(argv[1], "r");
		if (yyin == NULL) {
			return 2;
		}
		tokenizer::current_location.filename = std::string(argv[1]);
	}
	
	printf("<file:%s> ", tokenizer::current_location.filename.c_str());

	// As far as I remember, zero is EOF, negative is error and positive should be our token index.
	while (yylex() > 0);
	printf("\n");
	return 0;
}
