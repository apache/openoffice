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
 * lex grammar for parsing ressource source files (*.xrm files)
 * file is converted to gConXcu_yy.cxx with "flex"
 *****************************************************************************/


 
/***************   O V E R W R I T I N G   F U N C T I O N S   ***************/
%{
/* enlarge token buffer to tokenize whole std::strings */
#undef  YYLMAX
#define YYLMAX 64000

/* change reader function (input) to our own version */
#define YY_INPUT(buf,result,max_size) {int xres; IMPLptr->lexRead(buf, &xres, max_size); result = xres;}
%}



/*****************************   O P T I O N S   *****************************/
/* 8bit               --> allow 8bit characters in the input stream          */
/* noyywrap           --> yywrap is not called (single file scan)            */
/* never-interactive  --> no check for console output                        */
/* prefix=            --> yyFlexLexer change name                            */
/* --- The following options are for future use (maybe)                      */
/* yyclass=           --> subClass yyFlexLexer to allow own functions        */
/* c++                --> generate C++ classes                               */
%option 8bit noyywrap never-interactive
%array
%p 24000
%e 1200
%n 500



/***********************   H E L P E R   M A C R O S   ***********************/
SPACE [ \t]*
NAME  .*"oor:name="\"[^\"]+\"{SPACE}
FIN   [^/>]*">"
/*******************   R U L E S   D E F I N I T I O N S   *******************/
%%



"component-data" {
  LOCptr->addLevel();
  IMPLptr->copySource(yytext, false);
}



"<oor:component-data"{NAME} {
  LOCptr->addLevel();
  LOCptr->pushKey(yytext);
}



"<prop"{NAME}{FIN} |
"<node"{NAME}{FIN} {
  LOCptr->pushKey(yytext);
}



"</oor:component-data" |
"</prop"               |
"</node"               {
  LOCptr->popKey(yytext);
}


"<value xml:lang="\"[^\"]+\"[^>]*">" {
  LOCptr->startCollectData(yytext);
}



"</value>" {
  LOCptr->stopCollectData(yytext);
}



"&amp;"  |
"&apos;" |
"&gt;"   |
"&lt;"   |
"&quot;" {
  LOCptr->copySpecial(yytext);
}



({SPACE}\n{SPACE})+ {
  LOCptr->copyNL(yytext);
}



. {
  IMPLptr->copySource(yytext, LOCptr->mbNoCollectingData);

  // Just to please compiler.
  if (false)
    REJECT;
}



%%



void dummyJustForCompiler()
{
  char *txt = NULL;
  yy_flex_strlen(txt);
  yyunput(0, txt);
}
