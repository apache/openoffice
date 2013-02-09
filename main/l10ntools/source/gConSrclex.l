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

[tT][aA][bB][dD][iI][aA][lL][oO][gG][^\{]* {
  std::string text(yytext);
  convert_src::mcpImpl->pushKey(text);
}

[tT][aA][bB][cC][oO][nN][tT][rR][oO][lL][^\{]* {
  std::string text(yytext);
  convert_src::mcpImpl->pushKey(text);
}

[tT][aA][bB][pP][aA][gG][eE][^\{]* {
  std::string text(yytext);
  convert_src::mcpImpl->pushKey(text);
}

[fF][iI][xX][eE][dD][tT][eE][xX][tT][^\{]* {
  std::string text(yytext);
  convert_src::mcpImpl->pushKey(text);
}

[sS][tT][rR][iI][nN][gG][^\{]* {
  std::string text(yytext);
  convert_src::mcpImpl->pushKey(text);
}

[pP][aA][gG][eE][lL][iI][sS][tT][^\{]* {
  std::string text(yytext);
  convert_src::mcpImpl->pushNoKey(text);
}

[pP][aA][gG][eE][iI][tT][eE][mM][^\{]* {
  std::string text(yytext);
  convert_src::mcpImpl->pushNoKey(text);
}

[iI][dD][eE][nN][tT][iI][fF][iI][eE][rR][^;]* {
  std::string text(yytext);
  convert_src::mcpImpl->registerKey(text);
}

[tT][eE][xX][tT][^\n]* {
  std::string text(yytext);
  convert_src::mcpImpl->saveData(text);
}

"};" {
  std::string text(yytext);
  convert_src::mcpImpl->popKey(text);
}

.|\n {
  std::string text(yytext);
  convert_src::mcpImpl->copyData(text);
}

%%
