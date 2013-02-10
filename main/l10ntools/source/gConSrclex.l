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
#define YY_INPUT(buf,result,max_size) { convert_src::mcpImpl->lexRead(buf, &result, max_size); }
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



/*******************   R U L E S   D E F I N I T I O N S   *******************/
%%

[bB][iI][tT][mM][aA][pP][ ]+[a-zA-Z]+[^\{]*                          |
[bB][uU][tT][tT][oO][nN][iI][mM][aA][gG][eE][ ]+[a-zA-Z]+[^\{]*      |
[eE][dD][iI][tT][ ]+[a-zA-Z]+[^\{]*                                  |
[cC][aA][nN][cC][eE][lL][bB][uU][tT][tT][oO][nN][ ]+[a-zA-Z]+[^\{]*  |
[cC][oO][nN][tT][rR][oO][lL][ ]+[a-zA-Z]+[^\{]*                      |
[eE][rR][rR][oO][rR][bB][oO][xX][ ]+[a-zA-Z]+[^\{]*                  |
[fF][iI][xX][eE][dD][tT][eE][xX][tT][ ]+[a-zA-Z]+[^\{]*              |
[iI][mM][aA][gG][eE][ ]+[a-zA-Z]+[^\{]*                              |
[iI][mM][aA][gG][eE][bB][uU][tT][tT][oO][nN][ ]+[a-zA-Z]+[^\{]*      |
[iI][tT][eE][mM][lL][iI][sS][tT][ ]+[a-zA-Z]+[^\{]*                  |
[hH][eE][lL][pP][bB][uU][tT][oO][mM][ ]+[a-zA-Z]+[^\{]*              |
[mM][eE][nN][uU][ ]+[a-zA-Z]+[^\{]*                                  |
[mM][eE][nN][uU][iI][tT][eE][mM][ ]+[a-zA-Z]+[^\{]*                  |
[mM][oO][dD][aA][lL][dD][iI][aA][lL][oO][gG][ ]+[a-zA-Z]+[^\{]*      |
[mM][uU][lL][tT][iI][lL][iI][nN][eE][dD][iI][tT][ ]+[a-zA-Z]+[^\{]*  |
[pP][aA][gG][eE][iI][tT][eE][mM][ ]+[a-zA-Z]+[^\{]*                  |
[pP][aA][gG][eE][lL][iI][sS][tT][ ]+[a-zA-Z]+[^\{]*                  |
[pP][uU][sS][hH][bB][uU][tT][tT][oO][nN][ ]+[a-zA-Z]+[^\{]*          |
[sS][tT][rR][iI][nN][gG][ ]+[a-zA-Z]+[^\{]*                          |
[tT][aA][bB][cC][oO][nN][tT][rR][oO][lL][ ]+[a-zA-Z]+[^\{]*          |
[tT][aA][bB][dD][iI][aA][lL][oO][gG][ ]+[a-zA-Z]+[^\{]*              |
[tT][aA][bB][pP][aA][gG][eE][ ]+[a-zA-Z]+[^\{]*                      {
  std::string text(yytext);
  convert_src::mcpImpl->pushKey(text);
}



[bB][iI][tT][mM][aA][pP][ \t\n]+"{"                          |
[bB][uU][tT][tT][oO][nN][iI][mM][aA][gG][eE][ \t\n]+"{"      |
[eE][dD][iI][tT][ \t\n]+"{"                                  |
[cC][aA][nN][cC][eE][lL][bB][uU][tT][tT][oO][nN][ \t\n]"{"   |
[cC][oO][nN][tT][rR][oO][lL][ \t\n]+"{"                      |
[eE][rR][rR][oO][rR][bB][oO][xX][ \t\n]+"{"                  |
[fF][iI][xX][eE][dD][tT][eE][xX][tT][ \t\n]+"{"              |
[iI][mM][aA][gG][eE][ \t\n]+"{"                              |
[iI][mM][aA][gG][eE][bB][uU][tT][tT][oO][nN][ \t\n]+"{"      |
[iI][tT][eE][mM][lL][iI][sS][tT][ \t\n=]+"{"                  |
[hH][eE][lL][pP][bB][uU][tT][oO][mM][ \t\n]+"{"              |
[mM][eE][nN][uU][ \t\n]+"{"                                  |
[mM][eE][nN][uU][iI][tT][eE][mM][ \t\n]+"{"                  |
[mM][oO][dD][aA][lL][dD][iI][aA][lL][oO][gG][ \t\n]+"{"      |
[mM][uU][lL][tT][iI][lL][iI][nN][eE][dD][iI][tT][ \t\n]+"{"  |
[pP][aA][gG][eE][iI][tT][eE][mM][ \t\n]+"{"                  |
[pP][aA][gG][eE][lL][iI][sS][tT][ \t\n]+"{"                  |
[pP][uU][sS][hH][bB][uU][tT][tT][oO][nN][ \t\n]+"{"          |
[sS][tT][rR][iI][nN][gG][ \t\n]+"{"                          |
[tT][aA][bB][cC][oO][nN][tT][rR][oO][lL][ \t\n]+"{"          |
[tT][aA][bB][dD][iI][aA][lL][oO][gG][ \t\n]+"{"              |
[tT][aA][bB][pP][aA][gG][eE][ \t\n]+"{"                      {
  std::string text(yytext);
  convert_src::mcpImpl->pushNoKey(text);
}

[hH][eE][lL][pP][iI][dD][ \t]+'='[ \t]+[a-zA-Z]+[^;]*                 |
[iI][dD][eE][nN][tT][iI][fF][iI][eE][rR][ \t]*"="[ \t]*[a-zA-Z]+[^;]* {
  std::string text(yytext);
  convert_src::mcpImpl->registerKey(text);
}

[mM][eE][sS][sS][aA][gG][eE][ \t]*"["[ \t]*"en-US"[ \t]*"]"[ \t]+"="[^\n]*                         |
[qQ][uU][iI][cC][kK][hH][eE][lL][pP][tT][eE][xX][tT][ \t]*"["[ \t]*"en-US"[ \t]*"]"[ \t]+"="[^\n]* |
[tT][eE][xX][tT][ \t]*"["[ \t]*"en-US"[ \t]*"]"[ \t]+"="[^\n]*                                     {
  std::string text(yytext);
  convert_src::mcpImpl->saveData(text);
}

"}"[ ]*";" {
  std::string text(yytext);
  convert_src::mcpImpl->popKey(text);
}

.|\n {
  std::string text(yytext);
  convert_src::mcpImpl->copyData(text);
}

%%
