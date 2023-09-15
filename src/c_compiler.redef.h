#pragma once

// https://www.gnu.org/software/automake/manual/html_node/Linking-Multiple-Yacc-Parsers.html

#define yymaxdepth c_maxdepth
#define yyparse    c_parse
#define yylex      c_lex
#define yyerror    c_error
#define yylval     c_lval
#define yychar     c_char
#define yydebug    c_debug
#define yypact     c_pact
#define yyr1       c_r1
#define yyr2       c_r2
#define yydef      c_def
#define yychk      c_chk
#define yypgo      c_pgo
#define yyact      c_act
#define yyexca     c_exca
#define yyerrflag  c_errflag
#define yynerrs    c_nerrs
#define yyps       c_ps
#define yypv       c_pv
#define yys        c_s
#define yy_yys     c_yys
#define yystate    c_state
#define yytmp      c_tmp
#define yyv        c_v
#define yy_yyv     c_yyv
#define yyval      c_val
#define yylloc     c_lloc
#define yyreds     c_reds
#define yytoks     c_toks
#define yylhs      c_yylhs
#define yylen      c_yylen
#define yydefred   c_yydefred
#define yydgoto    c_yydgoto
#define yysindex   c_yysindex
#define yyrindex   c_yyrindex
#define yygindex   c_yygindex
#define yytable    c_yytable
#define yycheck    c_yycheck
#define yyname     c_yyname
#define yyrule     c_yyrule
