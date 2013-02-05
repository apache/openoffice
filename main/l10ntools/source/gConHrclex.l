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
 * lex grammar for parsing ressource source files (*.src and *.hrc files)
 * file is converted to gConHrc_yy.cxx with "flex"
 *****************************************************************************/



/***************   O V E R W R I T I N G   F U N C T I O N S   ***************/
%{
/* enlarge token buffer to tokenize whole strings */
#undef  YYLMAX
#define YYLMAX 64000

/* change reader function (input) to our own version */
#define YY_INPUT(buf,result,max_size) { convert_hrc::mcpImpl->lexRead(buf, &result, max_size); }
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

^[\t ]*"#pragma".*   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::PRAGMA, string(yytext)); }

^[ \t]*\n   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::EMPTYLINE, string(yytext)); }

[\t ]+               |
^[\t ]*"#include".*  |
^[\t ]*"#undef".*    |
"//".*               |
";"                  |
"<"                  |
">"                  |
\n   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::IGNORED, string(yytext)); }

"/*"   {
  convert_hrc::mcpImpl->addCommentToSet(convert_hrc_impl::COMMEND, string(yytext)); }

^[\t ]*"#ifndef".+$   |
^[\t ]*"#ifdef".+$    |
^[\t ]*"#if".+$       |
^[\t ]*"#elif".+$     |
^[\t ]*"#else".+$     |
^[\t ]*"#endif".+$    {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::CONDITION, string(yytext)); }

[a-zA-Z]+[\t ]+[^={\n]+[\t ]   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::DEFINEDRES, string(yytext)); /* defined Res */ }

[a-zA-Z]+[ \t]+[^={;\n]+\n[ \t]*"#".*\n[ \t]*"{"        |
[a-zA-Z]+[ \t]+[^={;\n]+\n?([ \t]*"//".*\n)*[ \t]*"{"   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::RESSOURCE, string(yytext)); /* RESSOURCE // String TTT_XX ... */ }

^[\t ]*[a-zA-Z_]+[\t ]*"\\"?[\t ]*\n?[ \t]*"{"[\t ]*"\\"?   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::SMALRESSOURCE, string(yytext)); /* SMALRESSOURCE // String ... */ }

[\t ]*[a-zA-Z0-9_]+[ \t]*("["[ \t]*[a-zA-Z0-9_\-]+[ \t]*"]"[ \t]*)?=[ \t]*L?\".*\".*\n?   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::TEXTLINE, string(yytext)); /* TEXTLINE // TextTyp = "A Text" */ }

[\t ]*[a-zA-Z0-9_]+[ \t]*("["[ \t]*[a-zA-Z0-9_\-]+[ \t]*"]"[ \t]*)?(\n[ \t]*)?=([ \t]*\n)?(([a-zA-Z0-9_]+)|(\".*\")|([ \t\n]*))*\".*\"(([a-zA-Z0-9_]+)|(\".*\")|([ \t\n]*))*;   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::LONGTEXTLINE, string(yytext)); /* LONGTEXTLINE // TextTyp = "A Text" HHH_XXX "A Text" ZZZ_TTT ... */ }

\".*\"   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::TEXT, string(yytext)); /* TEXT // "A Text" */ }

"{"[ \t]*\\?   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::LEVELUP, string(yytext)); /* LEVELUP */ }

"}"[ \t]*;([ \t]*\\)?   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::LEVELDOWN, string(yytext)); /* LEVELDOWN */ }

[a-zA-Z0-9_]+[ \t]*"="[ \t]*"MAP_APPFONT"[ \t]*"(".+")".*   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::APPFONTMAPPING, string(yytext)); /* APPFONTMAPPING  Typ = MAP_APPFONT( ... ) */ }

[ \t]*[a-zA-Z0-9_]+[ \t]*=[ \t]*[0123456789]{1,5}[ \t]*";"?\\?   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::TEXTREFID, string(yytext)); /* TEXTREFID // TextTyp = 12345 */ }

[a-zA-Z0-9_]+[ \t]*"=".*                          | 
[a-zA-Z0-9_]+[ \t]*"="[\t ]*([ \t]*"//".*\n)*.*   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::ASSIGNMENT, string(yytext)); /* ASSIGNMENT  Typ = ...  */ }

[a-zA-Z0-9_]+[ \t]*("["[ \t]*[a-zA-Z0-9_\-]+[ \t]*"]"[ \t]*)?"="[ \t]*(\\[ \t]*)?\n?[ \t]*"{"[ \t]*(\\[ \t]*)?\n?[ \t]*"<"   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::LISTASSIGNMENT, string(yytext)); /* LISTASSIGNMENT  Typ [ ... ] = ... */ }

"StringList"+[ \t]*("["[ \t]*[a-zA-Z0-9_\-]+[ \t]*"]"[ \t]*)?"="[ \t]*(\\[ \t]*)?\n?[ \t]*"{"[ \t]*(\\[ \t]*)?\n?[ \t]*   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::LISTASSIGNMENT, string(yytext)); /* LISTASSIGNMENT  Typ [ ... ] = ... */ }

"UIEntries"[ \t]*("["[ \t]*[a-zA-Z0-9_\-]+[ \t]*"]"[ \t]*)?"="[ \t]*(\\[ \t]*)?\n?[ \t]*"{"   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::UIENTRIES, string(yytext)); /* UIENTRIES */ }

"<"?[ \t]*L?\".*\".*">"   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::LISTTEXT, string(yytext)); /* LISTTEXT */ }

[ \t]*"#define"[ \t]+[a-zA-Z0-9_]+.*"\\"   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::RSCDEFINE, string(yytext)); /* RSCDEFINE  #define ... */ }

[ \t]*"#define"[ \t]+[a-zA-Z0-9_]+.+   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::NORMDEFINE, string(yytext)); /* #define ... */ }

"\\"   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::RSCDEFINELEND, string(yytext)); /* RSCDEFINELEND */ }

[a-zA-Z0-9_]+[ \t]*;   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::ANYTOKEN, string(yytext)); /* allowed other tokens like "49 ;" or "SFX_... ;" */ }

.   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::UNKNOWNCHAR, string(yytext)); /* YYWarning( "Unknown Char" ); */ }

"{"?[ \t]*\".*\"[ \t]*";"[ \t]*"}"   {
  convert_hrc::mcpImpl->addTokenToSet(convert_hrc_impl::_LISTTEXT, string(yytext)); /* _LISTTEXT */ }

%%
