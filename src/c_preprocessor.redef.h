#pragma once

// https://www.gnu.org/software/automake/manual/html_node/Linking-Multiple-Yacc-Parsers.html

#define yymaxdepth pp_maxdepth
#define yyparse    pp_parse
#define yylex      pp_lex
#define yyerror    pp_error
#define yylval     pp_lval
#define yychar     pp_char
#define yydebug    pp_debug
#define yypact     pp_pact
#define yyr1       pp_r1
#define yyr2       pp_r2
#define yydef      pp_def
#define yychk      pp_chk
#define yypgo      pp_pgo
#define yyact      pp_act
#define yyexca     pp_exca
#define yyerrflag  pp_errflag
#define yynerrs    pp_nerrs
#define yyps       pp_ps
#define yypv       pp_pv
#define yys        pp_s
#define yy_yys     pp_yys
#define yystate    pp_state
#define yytmp      pp_tmp
#define yyv        pp_v
#define yy_yyv     pp_yyv
#define yyval      pp_val
#define yylloc     pp_lloc
#define yyreds     pp_reds
#define yytoks     pp_toks
#define yylhs      pp_yylhs
#define yylen      pp_yylen
#define yydefred   pp_yydefred
#define yydgoto    pp_yydgoto
#define yysindex   pp_yysindex
#define yyrindex   pp_yyrindex
#define yygindex   pp_yygindex
#define yytable    pp_yytable
#define yycheck    pp_yycheck
#define yyname     pp_yyname
#define yyrule     pp_yyrule
