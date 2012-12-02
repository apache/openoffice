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
using namespace std;
#include "gConSrc.hxx"



/*****************************************************************************
 *********************   G C O N S R C U T I L . C X X   *********************
 *****************************************************************************
 * This is the conversion for .src and .hrc files
 * this is the actual conversion based on the lex tree
 *****************************************************************************/



/**********************   I M P L E M E N T A T I O N   **********************/
#if 0

TARGET
  utf8conv
  export2
  merge


transex3
  export
  srciter
  file
  directory
        srclex.l (src_yy)
        src_yy_wrapper

helpex
        helpmerge
        xmlparse
        helpex

ulfex
  lngmerge
        lngex

cfgex
  cfgmerge
        cfglex.l (cfg_yy)
        cfg_yy_wrapper

xrmex
        xrmmerge
        xrmlex.l (xrm_yy)
        xrm_yy_wrapper

xhtex
       <python>

xtxex
       <python>


.hrc .src    -->  transex3 -UTF8 -e      
.tree        -->  xhtex
m  .xtx         -->  xtxex
.ulf         -->  ulfex  -e
m  .xxl .xrb    -->  xmlex -UTF8 -e
m  .xgf         -->  xmlex -UTF8 -e -t:xgf
.xcu .xcd    -->  cfgex -UTF8 -e
.xcs         -->  cfgex -UTF8 -e -f
.xrm         -->  xrmex -UTF8 -e
.xhp         -->  helpex -e
.properties  -->  jpropex  -e


#endif
