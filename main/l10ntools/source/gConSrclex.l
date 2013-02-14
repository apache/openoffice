/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/


/*****************************************************************************
 **********************   L E X   D E F I N I T I O N   **********************
 *****************************************************************************
 * lex grammar for parsing ressource source files (*.src)
 * file is converted to gConSrc_yy.cxx with "flex"
 *****************************************************************************/



/***************   O V E R W R I T I N G   F U N C T I O N S   ***************/
%{
/* enlarge token buffer to tokenize whole std::strings */
#undef  YYLMAX
#define YYLMAX 64000

/* change reader function (input) to our own version */
#define YY_INPUT(buf,result,max_size) { IMPLptr->lexRead(buf, &result, max_size); }
%}



/*****************************   O P T I O N S   *****************************/
/* 8bit               --> allow 8bit characters in the input stream   */
/* noyywrap           --> yywrap is not called (single file scan)     */
/* never-interactive  --> no check for console output                 */
/* prefix=            --> yyFlexLexer change name                     */
/* --- The following options are for future use (maybe)               */
/* yyclass=           --> subClass yyFlexLexer to allow own functions */
/* c++                --> generate C++ classes                        */
%option 8bit noyywrap never-interactive prefix="genSrc_"
%p 24000
%e 1200
%n 500


/***********************   H E L P E R   M A C R O S   ***********************/
IDENT        [a-zA-Z0-9_]
PRE          ^[ \t]*
KEYPART      [ \t]+{IDENT}+[^\{]*"{"
NOKEYPART    ([ \t\n]+|[ \t\n]*"="[ \t\n]*)"{" 
REGKEYPART   [ \t]*"="[ \t]*[a-zA-Z\"]+[^;]*
GENSAVEPART  [ \t]*"["[ \t]*"en-US"[ \t]*"]"[ \t]*"="
SAVEKEYPART  {GENSAVEPART}[^\n]*
SAVEITEMPART {GENSAVEPART}[ \t\n]*[\\]*"\n"[ \t]*"{"[^\}]*"};"

/*******************   R U L E S   D E F I N I T I O N S   *******************/
%%


{PRE}[bB][iI][tT][mM][aA][pP]{KEYPART}                                  |
{PRE}[bB][uU][tT][tT][oO][nN][iI][mM][aA][gG][eE]{KEYPART}              |
{PRE}[cC][aA][nN][cC][eE][lL][bB][uU][tT][tT][oO][nN]{KEYPART}          |
{PRE}[cC][hH][eE][cC][kK][bB][oO][xX]{KEYPART}                          |
{PRE}[cC][oO][nN][tT][rR][oO][lL]{KEYPART}                              |
{PRE}[cC][oO][mM][bB][oO][bB][oO][xX]{KEYPART}                          |
{PRE}[eE][dD][iI][tT]{KEYPART}                                          |
{PRE}[eE][rR][rR][oO][rR][bB][oO][xX]{KEYPART}                          |
{PRE}[fF][iI][xX][eE][dD][tT][eE][xX][tT]{KEYPART}                      |
{PRE}[fF][iI][xX][eE][dD][lL][iI][nN][eE]{KEYPART}                      |
{PRE}[fF][lL][oO][aA][tT][iI][nN][gG][wW][iI][nN][dD][oO][wW]{KEYPART}  |
{PRE}[hH][eE][lL][pP][bB][uU][tT][tT][oO][nN]{KEYPART}                  |
{PRE}[iI][dD][lL][iI][sS][tT]{KEYPART}                                  |
{PRE}[iI][mM][aA][gG][eE]{KEYPART}                                      |
{PRE}[iI][mM][aA][gG][eE][lL][iI][sS][tT]{KEYPART}                      |
{PRE}[iI][mM][aA][gG][eE][bB][uU][tT][tT][oO][nN]{KEYPART}              |
{PRE}[iI][tT][eE][mM][lL][iI][sS][tT]{KEYPART}                          |
{PRE}[lL][iI][sS][tT][bB][oO][xX]{KEYPART}                              |
{PRE}[mM][eE][nN][uU]{KEYPART}                                          |
{PRE}[mM][eE][nN][uU][iI][tT][eE][mM]{KEYPART}                          |
{PRE}[mM][eE][tT][rR][iI][cC][fF][iI][eE][lL][dD]{KEYPART}              |
{PRE}[mM][oO][dD][aA][lL][dD][iI][aA][lL][oO][gG]{KEYPART}              |
{PRE}[mM][uU][lL][tT][iI][lL][iI][nN][eE][eE][dD][iI][tT]{KEYPART}      |
{PRE}[nN][uU][mM][eE][rR][iI][cC][fF][iI][eE][lL][dD]{KEYPART}          |
{PRE}[oO][kK][bB][uU][tT][tT][oO][nN]{KEYPART}                          |
{PRE}[pP][aA][gG][eE][iI][tT][eE][mM]{KEYPART}                          |
{PRE}[pP][aA][gG][eE][lL][iI][sS][tT]{KEYPART}                          |
{PRE}[pP][uU][sS][hH][bB][uU][tT][tT][oO][nN]{KEYPART}                  |
{PRE}[qQ][uU][eE][rR][yY][bB][oO][xX]{KEYPART}                          |
{PRE}[rR][aA][dD][iI][oO][bB][uU][tT][tT][oO][nN]{KEYPART}              |
{PRE}[rR][eE][sS][oO][uU][rR][cC][eE]{KEYPART}                          |
{PRE}[sS][pP][iI][nN][fF][iI][eE][lL][dD]{KEYPART}                      |
{PRE}[sS][tT][rR][iI][nN][gG]{KEYPART}                                  |
{PRE}[sS][tT][rR][iI][nN][gG][aA][rR][rR][aA][yY]{KEYPART}              |
{PRE}[tT][aA][bB][cC][oO][nN][tT][rR][oO][lL]{KEYPART}                  |
{PRE}[tT][aA][bB][dD][iI][aA][lL][oO][gG]{KEYPART}                      |
{PRE}[tT][aA][bB][pP][aA][gG][eE]{KEYPART}                              |
{PRE}[tT][oO][oO][lL][bB][oO][xX]{KEYPART}                              |
{PRE}[tT][oO][oO][lL][bB][oO][xX][iI][tT][eE][mM]{KEYPART}              |
{PRE}[tT][rR][iI][sS][tT][aA][tT][eE][bB][oO][xX]{KEYPART}              {
  LOCptr->pushKey(yytext);
}



{PRE}[bB][iI][tT][mM][aA][pP]{NOKEYPART}                                  |   
{PRE}[bB][uU][tT][tT][oO][nN][iI][mM][aA][gG][eE]{NOKEYPART}              |
{PRE}[cC][aA][nN][cC][eE][lL][bB][uU][tT][tT][oO][nN]{NOKEYPART}          |
{PRE}[cC][hH][eE][cC][kK][bB][oO][xX]{NOKEYPART}                          |
{PRE}[cC][oO][nN][tT][rR][oO][lL]{NOKEYPART}                              |
{PRE}[cC][oO][mM][bB][oO][bB][oO][xX]{NOKEYPART}                          |
{PRE}[eE][dD][iI][tT]{NOKEYPART}                                          |
{PRE}[eE][rR][rR][oO][rR][bB][oO][xX]{NOKEYPART}                          |
{PRE}[fF][iI][xX][eE][dD][tT][eE][xX][tT]{NOKEYPART}                      |
{PRE}[fF][iI][xX][eE][dD][lL][iI][nN][eE]{NOKEYPART}                      |
{PRE}[fF][lL][oO][aA][tT][iI][nN][gG][wW][iI][nN][dD][oO][wW]{NOKEYPART}  |
{PRE}[hH][eE][lL][pP][bB][uU][tT][tT][oO][nN]{NOKEYPART}                  |
{PRE}[iI][dD][lL][iI][sS][tT]{NOKEYPART}                                  |
{PRE}[iI][mM][aA][gG][eE]{NOKEYPART}                                      |
{PRE}[iI][mM][aA][gG][eE][bB][uU][tT][tT][oO][nN]{NOKEYPART}              |
{PRE}[iI][mM][aA][gG][eE][lL][iI][sS][tT]{NOKEYPART}                      |
{PRE}[iI][tT][eE][mM][lL][iI][sS][tT]{NOKEYPART}                          |
{PRE}[lL][iI][sS][tT][bB][oO][xX]{NOKEYPART}                              |
{PRE}[mM][eE][nN][uU]{NOKEYPART}                                          |
{PRE}[mM][eE][nN][uU][iI][tT][eE][mM]{NOKEYPART}                          |
{PRE}[mM][eE][tT][rR][iI][cC][fF][iI][eE][lL][dD]{NOKEYPART}              |
{PRE}[mM][oO][dD][aA][lL][dD][iI][aA][lL][oO][gG]{NOKEYPART}              |
{PRE}[mM][uU][lL][tT][iI][lL][iI][nN][eE][eE][dD][iI][tT]{NOKEYPART}      |
{PRE}[nN][uU][mM][eE][rR][iI][cC][fF][iI][eE][lL][dD]{NOKEYPART}          |
{PRE}[oO][kK][bB][uU][tT][tT][oO][nN]{NOKEYPART}                          |
{PRE}[pP][aA][gG][eE][iI][tT][eE][mM]{NOKEYPART}                          |
{PRE}[pP][aA][gG][eE][lL][iI][sS][tT]{NOKEYPART}                          |
{PRE}[pP][uU][sS][hH][bB][uU][tT][tT][oO][nN]{NOKEYPART}                  |
{PRE}[qQ][uU][eE][rR][yY][bB][oO][xX]{NOKEYPART}                          |
{PRE}[rR][aA][dD][iI][oO][bB][uU][tT][tT][oO][nN]{NOKEYPART}              |
{PRE}[rR][eE][sS][oO][uU][rR][cC][eE]{NOKEYPART}                          |
{PRE}[sS][pP][iI][nN][fF][iI][eE][lL][dD]{NOKEYPART}                      |
{PRE}[sS][tT][rR][iI][nN][gG]{NOKEYPART}                                  |
{PRE}[sS][tT][rR][iI][nN][gG][aA][rR][rR][aA][yY]{NOKEYPART}              |
{PRE}[tT][aA][bB][cC][oO][nN][tT][rR][oO][lL]{NOKEYPART}                  |
{PRE}[tT][aA][bB][dD][iI][aA][lL][oO][gG]{NOKEYPART}                      |
{PRE}[tT][aA][bB][pP][aA][gG][eE]{NOKEYPART}                              |
{PRE}[tT][oO][oO][lL][bB][oO][xX]{NOKEYPART}                              |
{PRE}[tT][oO][oO][lL][bB][oO][xX][iI][tT][eE][mM]{NOKEYPART}              |
{PRE}[tT][rR][iI][sS][tT][aA][tT][eE][bB][oO][xX]{NOKEYPART}              {
  LOCptr->pushNoKey(yytext);
}

[hH][eE][lL][pP][iI][dD]{REGKEYPART} {
  LOCptr->registerHelpKey(yytext);
}

[iI][dD][eE][nN][tT][iI][fF][iI][eE][rR]{REGKEYPART} {
  LOCptr->registerIdentKey(yytext);
}

[cC][uU][sS][tT][oO][mM][uU][nN][iI][tT][tT][eE][xX][tT]{SAVEKEYPART} |
[mM][eE][sS][sS][aA][gG][eE]{SAVEKEYPART}                             |
[qQ][uU][iI][cC][kK][hH][eE][lL][pP][tT][eE][xX][tT]{SAVEKEYPART}     |
[tT][eE][xX][tT]{SAVEKEYPART}                                         |
[tT][iI][tT][lL][eE]{SAVEKEYPART}                                     {
  LOCptr->saveData(yytext);
}

[iI][tT][eE][mM][lL][iI][sS][tT]{SAVEITEMPART}          |
[sS][tT][rR][iI][nN][gG][lL][iI][sS][tT]{SAVEITEMPART}  {
  LOCptr->saveItemList(yytext);
}

"}"[ ]*";" {
  LOCptr->popKey(yytext);
}

"{" {
  LOCptr->pushPlaceHolder(yytext);
}

"#define"[ \t]+[a-zA-Z_]+[ \t]+\\\n {
  LOCptr->startDefine(yytext);
}

"#define"[^\\\n]+\n |
"//".*\n            |
.|\n                {
  LOCptr->collectData(yytext);
}

%%
