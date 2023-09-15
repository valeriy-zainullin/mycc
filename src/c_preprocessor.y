/* Source of the syntax: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf, page 421, annex A: language syntax summary, part 2: phrase structure grammar, C99. */

// TODO: read about bison algorithm.

%define parse.error verbose

// TODO: figure out if this grammar is LR(1), maybe fix conflicts then, if LR(2), generating GLR parser is fine, no much overhead.
// https://stackoverflow.com/a/70601680
%glr-parser

%locations

%%

// Some time in future, too much work for now.

int preprocess(int argc, char** argv) {
	return execv("gcc", argv);
}
