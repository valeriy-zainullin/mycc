/* Source of the syntax: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf, page 421, annex A: language syntax summary, part 2: phrase structure grammar, C99. */

// TODO: read about bison algorithm.

#include "c_compiler.h"

#include <vector.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define report_error(msg) printf("syntax error: %s\n", msg); fflush(stdout); verify(false); // TODO: make it mark error in cc_state instead of using verify false.
#define verify(expr) if (!(expr)) { assert(false && "verify macro failed"); abort();}

enum cc_type_qualifiers {
	CC_TYPE_QUAL_CONST,
	CC_TYPE_QUAL_RESTRICT,
	CC_TYPE_QUAL_VOLATILE,
	CC_TYPE_QUAL_POINTER
};
// As bitmask for easier comparison
//   otherwise order would matter, etc.
typedef int cc_type_access_modifier;
static const cc_type_access_modifier cc_type_modifier_plain    = 0x0;
static const cc_type_access_modifier cc_type_modifier_const    = 0x1;
static const cc_type_access_modifier cc_type_modifier_restrict = 0x2;
static const cc_type_access_modifier cc_type_modifier_volatile = 0x4;
// unref_type * const volatile * const volatile = ...

enum cc_decl_scope {
	CC_DECL_GLOBAL,
	CC_DECL_LOCAL
};

struct cc_unref_type {
  enum cc_expr_type_kind {
		CC_BASE_TYPE,
		CC_TYPEDEF,
		CC_STRUCT,
		CC_ENUM
	} kind;

	enum cc_base_type {
		CC_BTYPE_VOID,
		CC_BTYPE_CHAR,
		CC_BTYPE_SINT,
		CC_BTYPE_USINT,
		CC_BTYPE_INT,
		CC_BTYPE_LINT,
		CC_BTYPE_LLINT,
		CC_BTYPE_UINT,
		CC_BTYPE_ULINT,
		CC_BTYPE_ULLINT
  } base_type;

	// Ref type is enum, typedef or struct name.
	enum   cc_decl_scope decl_scope; // where to look for the referenced type.
	size_t               decl_index; // index in the corresponding vector of decls
};

struct cc_expr_type {
  struct vector* ptr_mods;         // * [c v r] * [const volatile restrict] -- only modifiers are stored
  struct cc_unref_type unref_type; // Type after all pointers followed.
};

struct cc_typedef_decl {

};

struct cc_struct_field {
    char* name;
    struct cc_expr_type type;
};

struct cc_struct_decl {
    struct vector* fields; // vector of struct cc_struct_field.
};

// Always an int for now.
struct cc_enum_decl {
    struct vector* choices; // vector of char*
};

struct cc_type_decl {
	// What kind of type decl it is
	//   in source code.
	enum cc_type_decl_kind {
		CC_TYPE_DECL_TYPEDEF,
		CC_TYPE_DECL_STRUCT,
		CC_TYPE_DECL_ENUM
	} kind;
	union {
		struct cc_typedef_decl typedef_decl;
		struct cc_struct_decl  struct_decl;
		struct cc_enum_decl    enum_decl;
	};
	char* name; // struct [name], enum [name], typedef int [name];
};

struct cc_var_decl {
    char*               name;
    struct cc_expr_type type;
	uint32_t            base_offset; // [(rsp-used_stack) + base_offset] is location of the variable
};

struct cc_func_decl {
	char*               name;
	struct cc_expr_type ret_val_type;
	struct vector*      arg_types; // Made out of cc_expr_type.
};

struct cc_decl {
	enum cc_decl_kind {
		CC_DECL_KIND_TYPE,
		CC_DECL_KIND_VAR,
		CC_DECL_KIND_FUNC
    } kind;
	union {
		struct cc_type_decl type_decl;
		struct cc_var_decl  var_decl;
		struct cc_func_decl func_decl;
	};
};

struct cc_state {
    int    cur_token_id;
    char*  cur_token_text;

	struct vector* global_decls;  // made out of struct cc_func_def.
    struct vector* local_decls;   // made out of struct cc_local_var.
    uint32_t used_stack; // base + used_stack = rsp
};

int yylex();
extern char* yytext;
void yyerror(char const * string) {
    // (void) translation_unit;
    //(void) code_block_node;
    // (void) return_stmt;

    fprintf(stderr, "%s\n", string);
}
// At some point we'll tokenize ourselves, to become
//   reentrant and support preprocessing.
// With flex we'd have to redirect output of preprocessor
//   to a flex tokenizer.
int cc_read_token(struct cc_state* state) {
    state->cur_token_text = yytext;
    return yylex();
}

// Grammar specific types.

enum cc_syntax_decl {
	CC_SYNTAX_
};

// Use token table. Otherwise there are too many keywords and punctuators, code gets noticebly messier.
// We would have to provide separate functions for every keyword and punctuator, have many rules in the
// tokenizer, list every keyword and punctuator as separate rules in the rule section there. One repetition.
// Then function definitions in the standalone tokenizer, which just prints the tokens out to stdout. Not
// needed to figure out what particular keyword it is, also a repetition.
// And we would have to have them here, a lot more functions. Could make a macro, but token table and having
// general functions for keywords and punctuators are just better approach. No repetitons, only listed in
// table once and as tokens here, which we would have to do anyways if we would make separate functions for
// every keyword. We would have to declare those functions for handling keywords and punctuators in the
// "tokenizer.h", but then it's just too much work for every keyword and punctuator to have a function, they
// possibly declare it. It provokes to make a macro etc.
// %token-table
// Two links about token tables. yytoknum got removed, so yytname is not helpful. Define token list ourselves.
// 1. https://stackoverflow.com/a/43107075
// 2. https://dev.gnupg.org/T5616


/* TODO: keyword syntax here. */

enum cc_token {
    eof = 0,

    identifier,
    integer_constant, floating_constant, character_constant,
    string_literal,

	keyword_auto,     keyword_break,   keyword_case,     keyword_char,    keyword_const,
	keyword_continue, keyword_default, keyword_do,       keyword_double,  keyword_else,
	keyword_enum,     keyword_extern,  keyword_float,    keyword_for,     keyword_goto,
	keyword_if,       keyword_inline,  keyword_int,      keyword_long,    keyword_register,
	keyword_restrict, keyword_return,  keyword_short,    keyword_signed,  keyword_sizeof,
	keyword_static,   keyword_struct,  keyword_switch,   keyword_typedef, keyword_union,
	keyword_unsigned, keyword_void,    keyword_volatile, keyword_while,   keyword__Bool,
    keyword__Complex, keyword__Imaginary,

    punctuator_left_square_bracket,   punctuator_right_square_bracket,
    punctuator_left_parenthesis,      punctuator_right_parenthesis,
    punctuator_left_brace,            punctuator_right_brace,
    punctuator_dot,                   punctuator_arrow,
    punctuator_increment,             punctuator_decrement,
    punctuator_and,                   punctuator_star,
    punctuator_plus,                  punctuator_minus,
    punctuator_tilde,                 punctuator_exclamation_mark,
    punctuator_slash,                 punctuator_percent,
    punctuator_shift_left,   punctuator_shift_right,
    punctuator_less,   punctuator_greater,
    punctuator_less_equal,   punctuator_greater_equal,
    punctuator_equal_equal,   punctuator_not_equal,
    punctuator_circumflex,   punctuator_or,
    punctuator_and_and,   punctuator_or_or,
    punctuator_question_mark,   punctuator_colon,
    punctuator_semicolon,   punctuator_ellipsis,
    punctuator_equal,   punctuator_star_equal,
    punctuator_slash_equal,   punctuator_percent_equal,
    punctuator_plus_equal,   punctuator_minus_equal,
    punctuator_shift_left_equal,   punctuator_shift_right_equal,
    punctuator_and_equal,   punctuator_circumflex_equal,
    punctuator_or_equal,   punctuator_comma,
    punctuator_hash,   punctuator_hash_hash,
    punctuator_less_colon,   punctuator_colon_greater,
    punctuator_less_percent,   punctuator_percent_greater,
    punctuator_percent_colon,   punctuator_percent_colon_percent_colon
};

/*
TODO: standard reference here.
	[ ] ( ) { } . ->
	++ -- & * + - ~ !
	/ % << >> < > <= >= == != ^ | && ||
	? : ; ...
	= *= /= %= += -= <<= >>= &= ^= |=
	, # ##
	<: :> <% %> %: %:%:
*/

/* In order to defeat ambiguity in typedefs,
     we introduce tracking of context in
     tokenizer. If we're in a typedef,
     identifiers are treated as typenames.
   Otherwise it may be treated as
     init_declarator_list, variable name.
   Also, tokenizer for every identifier
     scans the list of typedefs to
     report identifiers that are
     types as actually types.
   Bison does lookahead, so we may have
     bugs, where bison gets tokens in
     lookahead, it 
*/


//%destructor { *output = $$; } <translation_unit>
// %destructor { if ($$ != NULL) { $$ = ast_external_decl_node_delete($$); } } <external_decl>
/*%destructor {
	if ($$ != *output) {
		$$ = ast_code_block_delete($$);
	}
} <code_block>*/

// More about destructors: https://stackoverflow.com/a/6401973
// I think, I will be able to figure out what to do, when I will be dealing with compilation errors.
// It's not now. It's some time in the future.
// %destructor { if ($$ != NULL) $$ = ast_return_stmt_delete($$); /* ast_jump_statement_delete($$); */ } <jump_statement>

// %destructor { if ($$ != NULL) $$ = ast_constant_delete($$); } <constant>
// %destructor { if ($$ != NULL) $$ = ast_int_constant_delete($$); } <int_constant>


/*
(6.5.1) primary-expression:
	identifier
	constant
	string-literal
	( expression )
*/

/*
(6.5.2) postfix-expression:
	primary-expression
	postfix-expression [ expression ]
	postfix-expression ( argument-expression-list_opt )
	postfix-expression . identifier
	postfix-expression -> identifier
	postfix-expression ++
	postfix-expression --
	( type-name ) { initializer-list }
	( type-name ) { initializer-list , }
*/
// Need to create a separate ast_init_list_expr, to not store initializer list information in
// postfix_expression structure, structure members get messy otherwise. Refactoring
// initializer lists into a separate rule so that syntax corresponds to ast type system.


/*
(6.5.2) argument-expression-list:
	assignment-expression
	argument-expression-list , assignment-expression
*/

/*
(6.5.3) unary-expression:
	postfix-expression
	++ unary-expression
	-- unary-expression
	unary-operator cast-expression
	sizeof unary-expression
	sizeof ( type-name )
*/

/*
(6.5.3) unary-operator: one of
	& * + - ~ !
*/

/*
(6.5.4) cast-expression:
	unary-expression
	( type-name ) cast-expression
*/

/*
(6.5.5) multiplicative-expression:
	cast-expression
	multiplicative-expression * cast-expression
	multiplicative-expression / cast-expression
	multiplicative-expression % cast-expression
*/

/*
(6.5.6) additive-expression:
	multiplicative-expression
	additive-expression + multiplicative-expression
	additive-expression - multiplicative-expression
*/

/*
(6.5.7) shift-expression:
	additive-expression
	shift-expression << additive-expression
	shift-expression >> additive-expression
*/

/*
(6.5.8) relational-expression:
	shift-expression
	relational-expression < shift-expression
	relational-expression > shift-expression
	relational-expression <= shift-expression
	relational-expression >= shift-expression
*/

/*
relational_expression:
  shift_expression:
    additive_expression:
  +
  shift_expression:
Количество пробелов -- длина макс оператора по длине + 1. indentation_level -> 
*/

/*
(6.5.9) equality-expression:
	relational-expression
	equality-expression == relational-expression
	equality-expression != relational-expression
*/

/*
(6.5.10) AND-expression:
	equality-expression
	AND-expression & equality-expression
*/

/*
(6.5.11) exclusive-OR-expression:
	AND-expression
	exclusive-OR-expression ^ AND-expression
*/

/*
(6.5.12) inclusive-OR-expression:
	exclusive-OR-expression
	inclusive-OR-expression | exclusive-OR-expression
*/

/*
(6.5.13) logical-AND-expression:
	inclusive-OR-expression
	logical-AND-expression && inclusive-OR-expression
*/

/*
(6.5.14) logical-OR-expression:
	logical-AND-expression
	logical-OR-expression || logical-AND-expression
*/

/*
(6.5.15) conditional-expression:
	logical-OR-expression
	logical-OR-expression ? expression : conditional-expression
*/

/*
(6.5.16) assignment-expression:
	conditional-expression
	unary-expression assignment-operator assignment-expression
*/

/*
(6.5.16) assignment-operator: one of
	= *= /= %= += -= <<= >>= &= ^= |=
*/

/*
(6.5.17) expression:
	assignment-expression
	expression , assignment-expression
*/

/*
(6.6) constant-expression:
	conditional-expression
*/

/*
(6.7) declaration-specifiers:
	storage-class-specifier declaration-specifiers_opt
	type-specifier declaration-specifiers_opt
	type-qualifier declaration-specifiers_opt
	function-specifier declaration-specifiers_opt
*/
/* TODO: storage class specifier can be typedef. I suppose it's invalid for function definitions. Check it on a different compiler, gcc maybe. If it is so, diallow typedef for functions in semantics. */
/* TODO: also not more than one same storage_class_specifier for a declaration. */


/*
(6.7) init-declarator-list:
	init-declarator
	init-declarator-list , init-declarator
*/

/*
(6.7) init-declarator:
	declarator
	declarator = initializer
*/


/*
(6.7.2.1) struct-declaration-list:
	struct-declaration
	struct-declaration-list struct-declaration
*/

/*
(6.7.2.1) struct-declaration:
	specifier-qualifier-list struct-declarator-list ;
*/

/*
(6.7.2.1) specifier-qualifier-list:
	type-specifier specifier-qualifier-list_opt
	type-qualifier specifier-qualifier-list_opt
*/

/*
(6.7.2.1) struct-declarator-list:
	struct-declarator
	struct-declarator-list , struct-declarator
*/

/*
(6.7.2.1) struct-declarator:
	declarator
	declarator_opt : constant-expression
*/

/*
(6.7.2.2) enum-specifier:
	enum identifier_opt { enumerator-list }
	enum identifier_opt { enumerator-list , }
	enum identifier
*/

/*
(6.7.2.2) enumerator-list:
	enumerator
	enumerator-list , enumerator
*/

/*
(6.7.2.2) enumerator:
	enumeration-constant
	enumeration-constant = constant-expression
*/
/* Enumeration constant is essentially an identifier. They will come to the parser as identifiers, otherwise we can't distinguish them at tokenizing stage. */
/*
enumerator:
  enumeration_constant
| enumeration_constant punctuator_equal constant_expression
;
*/

/*
(6.7.4) function-specifier:
	inline
*/

/*
(6.7.5) declarator:
	pointer_opt direct-declarator
*/

/*
(6.7.5) direct-declarator:
	identifier
	( declarator )
	direct-declarator [ type-qualifier-list_opt assignment-expression_opt ]
	direct-declarator [ static type-qualifier-list_opt assignment-expression ]
	direct-declarator [ type-qualifier-list static assignment-expression ]
	direct-declarator [ type-qualifier-list_opt * ]
	direct-declarator ( parameter-type-list )
	direct-declarator ( identifier-list_opt )
*/

/*
(6.7.5) pointer:
	* type-qualifier-list_opt
	* type-qualifier-list_opt pointer
*/

/*
(6.7.5) type-qualifier-list:
	type-qualifier
	type-qualifier-list type-qualifier
*/

/*
(6.7.5) parameter-type-list:
	parameter-list
	parameter-list , ...
*/

/*
(6.7.5) parameter-list:
	parameter-declaration
	parameter-list , parameter-declaration
*/

/*
(6.7.5) parameter-declaration:
	declaration-specifiers declarator
	declaration-specifiers abstract-declarator_opt
*/

/*
(6.7.5) identifier-list:
	identifier
	identifier-list , identifier
*/

/*
(6.7.6) type-name:
	specifier-qualifier-list abstract-declarator_opt
*/

/*
(6.7.6) abstract-declarator:
	pointer
	pointer_opt direct-abstract-declarator
*/

/*
(6.7.6) direct-abstract-declarator:
	( abstract-declarator )
	direct-abstract-declarator_opt [ type-qualifier-list_opt assignment-expression_opt ]
	direct-abstract-declarator_opt [ static type-qualifier-list_opt assignment-expression ]
	direct-abstract-declarator_opt [ type-qualifier-list static assignment-expression ]
	direct-abstract-declarator_opt [ * ]
	direct-abstract-declarator_opt ( parameter-type-list_opt )
*/
/* TODO: find an example of this! Are these some features of C I have never seen? */

/*
(6.7.7) typedef-name:
	identifier
*/

/*
(6.7.8) initializer:
	assignment-expression
	{ initializer-list }
	{ initializer-list , }
*/

/*
(6.7.8) initializer-list:
	designation_opt initializer
	initializer-list , designation_opt initializer
*/

/*
(6.7.8) designation:
	designator-list =
*/

/*
(6.7.8) designator-list:
	designator
	designator-list designator
*/

/*
(6.7.8) designator:
	[ constant-expression ]
	. identifier
*/

/*
(6.8) statement:
	labeled-statement
	compound-statement
	expression-statement
	selection-statement
	iteration-statement
	jump-statement
*/

/*
(6.8.1) labeled-statement:
	identifier : statement
	case constant-expression : statement
	default : statement
*/
/* TODO: 1st is ast_label_decl, 2nd is ast_case_decl, 3rd is ast_default_case_decl. */

/*
(6.8.2) compound-statement:
	{ block-item-list_opt }
*/

/*
(6.8.2) block-item-list:
	block-item
	block-item-list block-item
*/
/* TODO: named Block in printed output. */

/*
(6.8.2) block-item:
	declaration
	statement
*/

/*
(6.8.3) expression-statement:
	expression_opt ;
*/

/*
(6.8.4) selection-statement:
	if ( expression ) statement
	if ( expression ) statement else statement
	switch ( expression ) statement
*/

/*
(6.8.5) iteration-statement:
	while ( expression ) statement
	do statement while ( expression ) ;
	for ( expression_opt ; expression_opt ; expression_opt ) statement
	for ( declaration expression_opt ; expression_opt ) statement
*/

/*
(6.8.6) jump-statement:
	goto identifier ;
	continue ;
	break ;
	return expression_opt ;
*/
/* TODO: 1st is ast_goto_stmt, 2nd is continue_stmt, 3rd is break_stmt, 4th is return_stmt expression ptr*/

/*
(6.9.1) declaration-list:
	declaration
	declaration-list declaration
*/

// typedef can't have an init-declarator-list.
//   identifier in typedef is treated as a type name.
//   Not as a name of a variable.
/*
(6.7) declaration:
	declaration-specifiers init-declarator-list_opt ;
*/

/*
(6.9.1) function-definition:
	declaration-specifiers declarator declaration-list_opt compound-statement
*/

/*
(6.9) external-declaration:
	function-definition (пока -).
	declaration
==
	<function-definition> declaration-specifiers declarator declaration-list_opt compound-statement
	<declaration>         declaration-specifiers init-declarator-list_opt
==
	<function-definition> declaration-specifiers declarator declaration-list_opt compound-statement
	<declaration>         declaration-specifiers [init-declarator {init-declarator-list , init-declarator}]

	(6.7) init-declarator:
		declarator
		declarator = initializer
*/

// GCC doesn't allow these
//   to reappear in a decl.
// Actually, if we specify
//   a couple of these, it says
//   error: multiple storage classes in declaration specifiers
//   example (x86-64 gcc 13.2, you can experiment with it at godbolt.org):
//     static extern int a;
struct cc_stor_class {
    // All storage class specifiers
    bool is_typedef;
    bool is_extern;
    bool is_static;
    bool is_auto; // Not supported yet.
    bool is_register; // Ignored. Has no effect for codegen.
};
// Returns true, if consumed tokens.
bool cc_parse_stor_class(struct cc_state* state, struct cc_stor_class* stor_class) {
    assert(stor_class != NULL);

    if (state->cur_token_id == keyword_typedef) {
        if (stor_class->is_typedef) { report_error("duplicated typedef"); }
        stor_class->is_typedef = true;

        cc_read_token(state);
        return true;
    } else if (state->cur_token_id == keyword_extern) {
        if (stor_class->is_extern) { report_error("duplicated extern"); }
        stor_class->is_extern = true;

        cc_read_token(state);
        return true;
    } else if (state->cur_token_id == keyword_static) {
        if (stor_class->is_static) { report_error("duplicated static"); }
        stor_class->is_static = true;

        cc_read_token(state);
        return true;
    } else if (state->cur_token_id == keyword_auto || state->cur_token_id == keyword_register) {
        (void) stor_class->is_auto;
        (void) stor_class->is_register;
        assert(false);
        // Ignored for now.

        cc_read_token(state);
        return true;
    }

    return false;
}

/*
(6.9) translation-unit:
	external-declaration
	translation-unit external-declaration
*/

// typedef can't have an init-declarator-list.
//   identifier in typedef is treated as a type name.
//   Not as a name of a variable.
/*
(6.7) declaration:
	declaration-specifiers init-declarator-list_opt ;
(6.7.1) storage-class-specifier:
	typedef
	extern
	static
	auto
	register
(6.7.2) type-specifier:
    void
    char
    short
    int
    long
    float
    double
    signed
    unsigned
    _Bool
    _Complex
    struct-or-union-specifier
    enum-specifier
    typedef-name
(6.7.2.1) struct-or-union-specifier:
    struct-or-union identifier_opt { struct-declaration-list }
    struct-or-union identifier
(6.7.2.1) struct-or-union:
    struct
    union
(6.7.3) type-qualifier:
	const
	restrict
	volatile
(6.7) declaration-specifiers:
	storage-class-specifier declaration-specifiers_opt
	type-specifier declaration-specifiers_opt
	type-qualifier declaration-specifiers_opt
	function-specifier declaration-specifiers_opt
TODO: storage class specifier can be typedef. I suppose it's invalid for function definitions. Check it on a different compiler, gcc maybe. If it is so, diallow typedef for functions in semantics.
TODO: also not more than one same storage_class_specifier for a declaration.
*/
// It's only type and storage class.
//   So a decl spec may only have not more than
//   one identifier in it. And it's in the case
//   when we're dealing with a struct, union,
//   enum or a typedef, or a pointer to them.
//   They can be qualified at each level of
//   dereferencing, it's a given in this
//   discussion :)
struct cc_decl_specs {
    struct cc_stor_class stor_class;
    struct cc_expr_type  expr_type;
};
struct cc_decl_specs* decl_specs_new() {
    struct cc_decl_specs* decl_specs = calloc(1, sizeof(struct cc_decl_specs));
    if (decl_specs == NULL) {
        return NULL;
    }
    /*if (!decl_specs_init(decl_specs)) {
        free(decl_specs);
        return NULL;
    }*/
    return decl_specs;
}

struct cc_type_spec {
    // Type specifier.
    // After enums and structs next token is identifier, it's name. TODO: locate this in the c99 standard as wording.
    bool is_base_type;

    bool is_signed;       // Can't be signed and unsigned at the same time, signed decl spec can't be duplicated (gcc doesn't allow that also).
    bool is_unsigned;     // If we had unsigned or not. We do not allow duplicate unsigned (gcc doesn't, it knows better).
    bool is_short;        // If we had short in specifiers.
    int  long_level;      // How many longs in specifiers.
    bool is_int;

    bool is_void;
    bool is_char;
    bool is_float;
    bool is_double;

    bool is_struct;
    bool is_enum;
    bool is_union;
    char* name;     // Struct, enum or union name.
    // Not supported yet, we'd reuse struct, union and enum parsing here.
    // size_t immediate_id; // Type id, in case it's an immediate definition.
};

void cc_parse_decl_specs(struct cc_state* state) {
    struct cc_stor_class stor_class = {0};
    stor_class.is_typedef = false;
    stor_class.is_extern = false;
    stor_class.is_static = false;
    stor_class.is_auto = false;
    stor_class.is_register = false;

    bool had_pointers = false;

    struct cc_type_spec type_spec = {0};

    struct cc_expr_type* expr_type = NULL;
	// Type qualifiers.
    struct vector* type_qualifiers = vector_new(0, sizeof(cc_type_access_modifier)); // Vector of enum cc_type_modifiers.
    verify(type_qualifiers != NULL);
	// Function specifier.
    bool is_inline = false; // It means we're dealing with a function.

	do {
        if (cc_parse_stor_class(state, &stor_class)) {
            continue;
        } else if (state->cur_token_id == keyword_const) {
            int qualifier = CC_TYPE_QUAL_CONST;
            verify(vector_push(type_qualifiers, &qualifier));
        } else if (state->cur_token_id == keyword_restrict) {
            int qualifier = CC_TYPE_QUAL_RESTRICT;
            verify(vector_push(type_qualifiers, &qualifier));
        } else if (state->cur_token_id == keyword_volatile) {
            int qualifier = CC_TYPE_QUAL_VOLATILE;
            verify(vector_push(type_qualifiers, &qualifier));
        } else if (state->cur_token_id == punctuator_star) {
            // Pointer can't break a type specifier. It has to be after it in scanning.
            //   GCC doesn't allow that.
            //   It's sensible that it doesn't allow that, what meaning
            //   would that have, when reading? Kind of a bug in syntax, it could feel as.
            //   example (gcc 13.2):
            //       int unsigned * long b4;
            //       struct new_struct * { int a1; int a2;};
            int qualifier = CC_TYPE_QUAL_POINTER;
            verify(vector_push(type_qualifiers, &qualifier));
        } else if (state->cur_token_id == keyword_inline) {
			if (is_inline) { report_error("duplicated inline"); }
			is_inline = true;
        } else if (state->cur_token_id == keyword_unsigned) {

        } else if (state->cur_token_id == keyword_struct) {
            if (type_spec.is_struct || type_spec.is_enum || type_spec.is_union) { report_error("duplicated enum or struct or union, only one per type."); }
            type_spec.is_struct = true;
            // Next token is struct name or immediate declaration.
            // We may have both parts.
            // If struct name is empty, it must have a definition.
            //   We may check that if we just have a struct decl as global or a declaration in a function (without creating an immediate instance),
            //   but the struct itself doesn't have a name, we may warn: unnamed struct doesn't produce instances. That init list in decl syntax is for creating instances. Used for variables and type decls.
            // If struct has a name, but is not defined neither here, nor previously, we just mark is as a struct, declared, but not defined. It's allowed, a forward declaration it's called.
            //   If it's used to create instanced though, we should refuse to do so. It's reasonable: what would this instance consist of? We may make it an empty struct, but we should let the user know he may be doing something he didn't intend the program to do.
            cc_read_token(state);
            bool definition_is_mandatory = true;
            if (state->cur_token_id == identifier) {
                definition_is_mandatory = false;
                char* name = calloc(strlen(state->cur_token_text) + 1, sizeof(char));
                verify(name != NULL);
                strcpy(name, state->cur_token_text);
                type_spec.name = name;
                cc_read_token(state);
            }
            // (Possibly, immediate) definition
            if (state->cur_token_id == punctuator_left_brace) {
                // TODO: process struct definition.
            } else if (definition_is_mandatory) {
                // TODO: report an error, struct both without a name and an immediate definition.
                verify(false);
            }

            // Right now we don't support struct decls.
            // TODO.
        } else if (state->cur_token_id == keyword_enum) {
            if (type_spec.is_struct || type_spec.is_enum || type_spec.is_union) { report_error("duplicated enum or struct or union, only one per type."); }
            type_spec.is_enum = true;
            cc_read_token(state);
            // TODO.
        } else if (state->cur_token_id == identifier && expr_type == NULL) {
            // Only one type name is allowed in type specifiers.

			// In every declaration there's at least
			//   one type.
			// In new type declaration old_type new_type,
			//   so old_type is read, new_type is an identifier.
			// In function ptr decl int*const*volatile** (*f)();
			//   int*const*volatile** is type read. Later we
			//   figure out it's a function decl with such a
			//   return type.
			// Not to mention a variable decl.
			char* type_name = yytext;

			// For typedef this identifier

			// Look for this type in decls.
			for (size_t local_decl_index = 0; local_decl_index < vector_get_size(state->local_decls); ++local_decl_index) {
                struct cc_decl* decl = NULL;
                vector_get(state->local_decls, local_decl_index, &decl);
                assert(decl != NULL); // Sanity check, we don't store nullptrs there.

                if (decl->kind != CC_DECL_KIND_TYPE) {
					continue;
				}

				printf("Found a decl.\n");
			}
		} else break; // Decl specifiers ended.
    } while (yylex() != eof);
}

void cc_parse_decl(struct cc_state* state) {
    cc_parse_decl_specs(state);
}

void cc_parse_ext_decl(struct cc_state* state) {
	cc_parse_decl(state);
}

void cc_parse_tunit(struct cc_state* state) {
	// Мы здесь ещё и пустые translation unit разрешаем,
	//   ничего страшного.
    //while (yylex() != 0) {//(!state->eof) {
		cc_parse_ext_decl(state);
    //}
}

#include <stdbool.h>
#include <stddef.h>

#define VERIFY(EXPR) if (!(EXPR)) { printf("File %s, line %d: expr %s is false. Program was terminated.\n", __FILE__, __LINE__, #EXPR); abort(); }

struct token_table_entry {
	char const* token_text;
	int symbol_index;
};

#define TO_STRING2(value) #value
#define TO_STRING(value) TO_STRING2(value)

#define KEYWORD_ENTRY(name) { #name , keyword_ ## name }
#define PUNCTUATOR_ENTRY(text, name) { text, punctuator_ ## name }

static const struct token_table_entry token_table[] = {
	KEYWORD_ENTRY(auto),     KEYWORD_ENTRY(break),    KEYWORD_ENTRY(case),     KEYWORD_ENTRY(char),
	KEYWORD_ENTRY(const),    KEYWORD_ENTRY(continue), KEYWORD_ENTRY(default),  KEYWORD_ENTRY(do),
	KEYWORD_ENTRY(double),   KEYWORD_ENTRY(else),     KEYWORD_ENTRY(enum),     KEYWORD_ENTRY(extern),
	KEYWORD_ENTRY(float),    KEYWORD_ENTRY(for),      KEYWORD_ENTRY(goto),     KEYWORD_ENTRY(if),
	KEYWORD_ENTRY(inline),   KEYWORD_ENTRY(int),      KEYWORD_ENTRY(long),     KEYWORD_ENTRY(register),
	KEYWORD_ENTRY(restrict), KEYWORD_ENTRY(return),   KEYWORD_ENTRY(short),    KEYWORD_ENTRY(signed),
	KEYWORD_ENTRY(sizeof),   KEYWORD_ENTRY(static),   KEYWORD_ENTRY(struct),   KEYWORD_ENTRY(switch),
	KEYWORD_ENTRY(typedef),  KEYWORD_ENTRY(union),    KEYWORD_ENTRY(unsigned), KEYWORD_ENTRY(void),
	KEYWORD_ENTRY(volatile), KEYWORD_ENTRY(while),    KEYWORD_ENTRY(_Bool),    KEYWORD_ENTRY(_Complex),
	KEYWORD_ENTRY(_Imaginary),
	
	PUNCTUATOR_ENTRY(  "[", left_square_bracket),  PUNCTUATOR_ENTRY(   "]",        right_square_bracket),
	PUNCTUATOR_ENTRY(  "(",    left_parenthesis),  PUNCTUATOR_ENTRY(   ")",           right_parenthesis),
	PUNCTUATOR_ENTRY(  "{",          left_brace),  PUNCTUATOR_ENTRY(   "}",                 right_brace),
	PUNCTUATOR_ENTRY(  ".",                 dot),  PUNCTUATOR_ENTRY(  "->",                       arrow),
	PUNCTUATOR_ENTRY( "++",           increment),  PUNCTUATOR_ENTRY(  "--",                   decrement),
	PUNCTUATOR_ENTRY(  "&",                 and),  PUNCTUATOR_ENTRY(   "*",                        star),
	PUNCTUATOR_ENTRY(  "+",                plus),  PUNCTUATOR_ENTRY(   "-",                       minus),
	PUNCTUATOR_ENTRY(  "~",               tilde),  PUNCTUATOR_ENTRY(   "!",            exclamation_mark),
	PUNCTUATOR_ENTRY(  "/",               slash),  PUNCTUATOR_ENTRY(   "%",                     percent),
	PUNCTUATOR_ENTRY( "<<",          shift_left),  PUNCTUATOR_ENTRY(  ">>",                 shift_right),
	PUNCTUATOR_ENTRY(  "<",                less),  PUNCTUATOR_ENTRY(   ">",                     greater),
	PUNCTUATOR_ENTRY( "<=",          less_equal),  PUNCTUATOR_ENTRY(  ">=",               greater_equal),
	PUNCTUATOR_ENTRY( "==",         equal_equal),  PUNCTUATOR_ENTRY(  "!=",                   not_equal),
	PUNCTUATOR_ENTRY(  "^",          circumflex),  PUNCTUATOR_ENTRY(   "|",                          or),
	PUNCTUATOR_ENTRY( "&&",             and_and),  PUNCTUATOR_ENTRY(  "||",                       or_or),
	PUNCTUATOR_ENTRY(  "?",       question_mark),  PUNCTUATOR_ENTRY(   ":",                       colon),
	PUNCTUATOR_ENTRY(  ";",           semicolon),  PUNCTUATOR_ENTRY( "...",                    ellipsis),
	PUNCTUATOR_ENTRY(  "=",               equal),  PUNCTUATOR_ENTRY(  "*=",                  star_equal),
	PUNCTUATOR_ENTRY( "/=",         slash_equal),  PUNCTUATOR_ENTRY(  "%=",               percent_equal),
	PUNCTUATOR_ENTRY( "+=",          plus_equal),  PUNCTUATOR_ENTRY(  "-=",                 minus_equal),
	PUNCTUATOR_ENTRY("<<=",    shift_left_equal),  PUNCTUATOR_ENTRY( ">>=",           shift_right_equal),
	PUNCTUATOR_ENTRY( "&=",           and_equal),  PUNCTUATOR_ENTRY(  "^=",            circumflex_equal),
	PUNCTUATOR_ENTRY( "|=",            or_equal),  PUNCTUATOR_ENTRY(   ",",                       comma),
	PUNCTUATOR_ENTRY(  "#",                hash),  PUNCTUATOR_ENTRY(  "##",                   hash_hash),
	PUNCTUATOR_ENTRY( "<:",          less_colon),  PUNCTUATOR_ENTRY(  ":>",               colon_greater),
	PUNCTUATOR_ENTRY( "<%",        less_percent),  PUNCTUATOR_ENTRY(  "%>",             percent_greater),
	PUNCTUATOR_ENTRY( "%:",       percent_colon),  PUNCTUATOR_ENTRY("%:%:", percent_colon_percent_colon)
};

#define NUM_TOKENS sizeof(token_table) / sizeof(struct token_table_entry)

#undef KEYWORD_ENTRY
#undef PUNCTUATOR_ENTRY

static int find_literal_token(char const* text) {
	for (size_t i = 0; i < NUM_TOKENS; ++i) {
		if (strcmp(token_table[i].token_text, text) == 0) {
			return token_table[i].symbol_index;
		}
	}
	
	VERIFY(false);
	__builtin_unreachable();
}

int cc_handle_keyword(char const* text) {
    printf("%s ", text);
    fflush(stdout);
    return find_literal_token(text);
}

int cc_handle_punctuator(char const* text) {
    printf("%s ", text);
    fflush(stdout);
    return find_literal_token(text);
}

int cc_handle_identifier(char const* text) {
	(void) text;
    printf("%s ", text);
    fflush(stdout);

	return identifier;
}

int cc_handle_integer_constant(char const* text) {
	(void) text;
	// yylval.int_constant = ast_int_constant_new_from_token(text);
	// TODO: check for null pointer, returned if an allocation fails.
    printf("%s ", text);
    fflush(stdout);

	return integer_constant;
}

int cc_handle_floating_constant(char const* text) {
	(void) text;
    printf("%s ", text);
    fflush(stdout);

	return floating_constant;
}

int cc_handle_character_constant(char const* text) {
	(void) text;
	printf("%s ", text);
    fflush(stdout);
	
	return character_constant;
}

int cc_handle_string_literal(char const* text) {
	(void) text;
    printf("%s ", text);
    fflush(stdout);

	return string_literal;
}

int cc_handle_invalid_token(char const* text) {
	(void) text;
	printf("%s ", text);
	
	return -1;
}

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

bool run_preprocessor(int argc, char** argv) {
	char preprocessed_file[] = "mycc.preprecessed.XXXXXX";
	int preprocessed_fd = mkstemp(preprocessed_file);

	if (preprocessed_fd == -1) {
		perror(NULL);
		return false;
	}

	printf("tmp file created.\n");

	// filepath (argv[0]), -E, -o ..., NULL
	size_t new_argv_size = argc + 5;
	char** new_argv = calloc(argc + 5, sizeof(char*));
	if (new_argv == NULL) {
		unlink(preprocessed_file);
		close(preprocessed_fd);
		return false;
	}

	printf("allocated new argv.\n");

	new_argv[0] = "gcc";
	new_argv[1] = "-E";
	new_argv[2] = "-o";
	new_argv[3] = preprocessed_file;
	// size_t heap_argv_start = 4;
	size_t new_argv_pos = 4;

	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-c") == 0) {
			continue;
		} else if (strncmp(argv[i], "-o", 2) == 0) {
			if (strcmp(argv[i], "-o") == 0) {
				i += 1; // Skip output filename.
				continue;
			} else {
				// file name is straight after -o. Just continue
				continue;
			}
		}

		new_argv[new_argv_pos] = argv[i];
		new_argv_pos += 1;
	}
	assert(new_argv_pos < new_argv_size);
	new_argv[new_argv_pos] = NULL;

	printf("executing gcc..\n");

	int status = 0;
	int pid = fork(); // 0, if we're in child, otherwise child's pid.

	if (pid == 0) {
		errno = 0;

        for (int i = 0; i <= argc; ++i) {
            printf("argv[%d] = '%s'\n", i, argv[i]);
        }
        fflush(stdout);

		for (size_t i = 0; i <= new_argv_pos; ++i) {
			printf("new_argv[%zu] = '%s'\n", i, new_argv[i]);
		}
        fflush(stdout);

		if (execvp("/usr/bin/gcc", new_argv) != 0) {
			if (errno != 0) {
				// We can get there if execv fails or gcc itself returns not zero. So set errno to zero beforehand to not get previous error msg, when gcc failed, not the execve.
				perror(NULL);
			}
			exit(1);
		}
		exit(0); // Exit the child process.
	} else {
		int wait_status = waitpid(pid, &status, 0);
		if (wait_status == -1 || WEXITSTATUS(status) != 0) {
			if (wait_status == -1) {
				perror(NULL);
			}
			free(new_argv);
			unlink(preprocessed_file);
			close(preprocessed_fd);
			return false;
		}
	}

	printf("executed gcc.\n");

	freopen(preprocessed_file, "r", stdin);

	// File is not in the filetree anymore, deleted after our process exits.
    // Closed by the os on our exit.
    unlink(preprocessed_file);

	free(new_argv);

	return true;
}

int main(int argc, char** argv) {
	if (!run_preprocessor(argc, argv)) {
		return 1;
	}

    struct cc_state state;

    cc_read_token(&state);

    cc_parse_tunit(&state);

	return 0;
}
