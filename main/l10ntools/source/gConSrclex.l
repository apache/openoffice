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
%option 8bit noyywrap never-interactive
%array
%p 24000
%e 1200
%n 500


/***********************   H E L P E R   M A C R O S   ***********************/
%x CMD
PRE   ^[ \t]*
SUF   [ \t\r\n]
SUFT  [ \t\r\n\[]
SPACE [ \t]*
IDENT ([a-zA-Z0-9][ a-zA-Z0-9_\-\+\*]*[a-zA-Z0-9]|[0-9])
KEYID [a-zA-Z0-9_-]+

/*******************   R U L E S   D E F I N I T I O N S   *******************/
%%



"/*" {
  int i = 1;
  for (;;)
  {
    while ((yytext[++i] = yyinput()) != '*') ;
    if ((yytext[++i] = yyinput()) == '/')
    break;
  }
  yytext[i] = '\0';

  IMPLptr->copySource(yytext);
}



"//".* {
  IMPLptr->copySource(yytext);
}



"\"" {
  int i = 0;
  for (; (yytext[++i] = yyinput()) != '\"';)
    if (yytext[i] == '\\')
      yytext[++i] = yyinput();
  yytext[i] = '\0';

  LOCptr->setValue(&yytext[1]);
}



"{" {
  LOCptr->startBlock(yytext);
}



"}"{SPACE}";"* {
  LOCptr->stopBlock(yytext);
}



{PRE}"<"{SUF} {
  yyless(strlen(yytext)-1);
  LOCptr->setListItem(yytext, true);
}



">"{SPACE}";"{SUF} {
  yyless(strlen(yytext)-1);
  LOCptr->setListItem(yytext, false);
}



\n {
  LOCptr->setNL(yytext, false);
}



\\[\r]*\n {
  LOCptr->setNL(yytext, true);
}



"["{SPACE}en-US{SPACE}"]" {
  LOCptr->setLang(yytext, true);
}



"["{SPACE}{KEYID}{SPACE}"]" {
  LOCptr->setLang(yytext, false);
}



{PRE}[bB][iI][tT][mM][aA][pP]{SUF}                                          |
{PRE}[bB][uU][tT][tT][oO][nN][iI][mM][aA][gG][eE]{SUF}                      |
{PRE}[cC][aA][nN][cC][eE][lL][bB][uU][tT][tT][oO][nN]{SUF}                  |
{PRE}[cC][hH][eE][cC][kK][bB][oO][xX]{SUF}                                  |
{PRE}[cC][oO][nN][tT][rR][oO][lL]{SUF}                                      |
{PRE}[cC][oO][mM][bB][oO][bB][oO][xX]{SUF}                                  |
{PRE}[eE][dD][iI][tT]{SUF}                                                  |
{PRE}[eE][rR][rR][oO][rR][bB][oO][xX]{SUF}                                  |
{PRE}[fF][iI][xX][eE][dD][tT][eE][xX][tT]{SUF}                              |
{PRE}[fF][iI][xX][eE][dD][lL][iI][nN][eE]{SUF}                              |
{PRE}[fF][lL][oO][aA][tT][iI][nN][gG][wW][iI][nN][dD][oO][wW]{SUF}          |
{PRE}[hH][eE][lL][pP][bB][uU][tT][tT][oO][nN]{SUF}                          |
{PRE}[iI][dD][lL][iI][sS][tT]{SUF}                                          |
{PRE}[iI][mM][aA][gG][eE]{SUF}                                              |
{PRE}[iI][mM][aA][gG][eE][lL][iI][sS][tT]{SUF}                              |
{PRE}[iI][mM][aA][gG][eE][bB][uU][tT][tT][oO][nN]{SUF}                      |
{PRE}[iI][mM][aA][gG][eE][rR][aA][dD][iI][oO][bB][uU][tT][tT][oO][nN]{SUF}  |
{PRE}[iI][nN][fF][oO][bB][oO][xX]{SUF}                                      |
{PRE}[lL][iI][sS][tT][bB][oO][xX]{SUF}                                      |
{PRE}[mM][eE][nN][uU]{SUF}                                                  |
{PRE}[mM][eE][nN][uU][bB][uU][tT][tT][oO][nN]{SUF}                          |
{PRE}[mM][eE][nN][uU][iI][tT][eE][mM]{SUF}                                  |
{PRE}[mM][eE][tT][rR][iI][cC][fF][iI][eE][lL][dD]{SUF}                      |
{PRE}[mM][oO][dD][aA][lL][dD][iI][aA][lL][oO][gG]{SUF}                      |
{PRE}[mM][oO][dD][eE][lL][eE][sS][sS][dD][iI][aA][lL][oO][gG]{SUF}          |
{PRE}[mM][uU][lL][tT][iI][lL][iI][nN][eE][eE][dD][iI][tT]{SUF}              |
{PRE}[nN][uU][mM][eE][rR][iI][cC][fF][iI][eE][lL][dD]{SUF}                  |
{PRE}[oO][kK][bB][uU][tT][tT][oO][nN]{SUF}                                  |
{PRE}[pP][aA][gG][eE][iI][tT][eE][mM]{SUF}                                  |
{PRE}[pP][aA][gG][eE][lL][iI][sS][tT]{SUF}                                  |
{PRE}[pP][uU][sS][hH][bB][uU][tT][tT][oO][nN]{SUF}                          |
{PRE}[qQ][uU][eE][rR][yY][bB][oO][xX]{SUF}                                  |
{PRE}[rR][aA][dD][iI][oO][bB][uU][tT][tT][oO][nN]{SUF}                      |
{PRE}[rR][eE][sS][oO][uU][rR][cC][eE]{SUF}                                  |
{PRE}[sS][pP][iI][nN][fF][iI][eE][lL][dD]{SUF}                              |
{PRE}[sS][tT][rR][iI][nN][gG]{SUF}                                          |
{PRE}[sS][tT][rR][iI][nN][gG][aA][rR][rR][aA][yY]{SUF}                      |
{PRE}[tT][aA][bB][cC][oO][nN][tT][rR][oO][lL]{SUF}                          |
{PRE}[tT][aA][bB][dD][iI][aA][lL][oO][gG]{SUF}                              |
{PRE}[tT][aA][bB][pP][aA][gG][eE]{SUF}                                      |
{PRE}[tT][oO][oO][lL][bB][oO][xX]{SUF}                                      |
{PRE}[tT][oO][oO][lL][bB][oO][xX][iI][tT][eE][mM]{SUF}                      |
{PRE}[tT][rR][iI][sS][tT][aA][tT][eE][bB][oO][xX]{SUF}                      |
{PRE}[wW][aA][rR][nN][iI][nN][gG][bB][oO][xX]{SUF}                          |
{PRE}[wW][iI][nN][dD][oO][wW]{SUF}                                          {
  yyless(strlen(yytext)-1);
  LOCptr->setCmd(yytext);
  BEGIN(CMD);
}

<CMD>{IDENT} {
  LOCptr->setName(yytext);
  BEGIN(INITIAL);
}

<CMD>[ \t=]+ {
  IMPLptr->copySource(yytext);
}

<CMD>[\\\n/] {
  yyless(1);
  BEGIN(INITIAL);
}



{PRE}[hH][eE][lL][pP][iI][dD]{SUF} {
  yyless(strlen(yytext)-1);
  LOCptr->setId(yytext, false);
}



{PRE}[iI][dD][eE][nN][tT][iI][fF][iI][eE][rR]{SUF} {
  yyless(strlen(yytext)-1);
  LOCptr->setId(yytext, true);
}



{PRE}[cC][uU][sS][tT][oO][mM][uU][nN][iI][tT][tT][eE][xX][tT]{SUFT} |
{PRE}[mM][eE][sS][sS][aA][gG][eE]{SUFT}                             |
{PRE}[qQ][uU][iI][cC][kK][hH][eE][lL][pP][tT][eE][xX][tT]{SUFT}     |
{PRE}[tT][eE][xX][tT]{SUFT}                                         |
{PRE}[tT][iI][tT][lL][eE]{SUFT}                                     {
  yyless(strlen(yytext)-1);
  LOCptr->setText(yytext);
}



{PRE}[iI][tT][eE][mM][lL][iI][sS][tT]{SUFT} {
  yyless(strlen(yytext)-1);
  LOCptr->setList(yytext);
}



{PRE}[sS][tT][rR][iI][nN][gG][lL][iI][sS][tT]{SUFT}  {
  yyless(strlen(yytext)-1);
  LOCptr->setStringList(yytext);
}



{PRE}"#define"{SUF} {
  yyless(strlen(yytext)-1);
  LOCptr->setMacro(yytext);
}



{KEYID} {
  LOCptr->setName(yytext);
}



. {
  IMPLptr->copySource(yytext);
}

%%
