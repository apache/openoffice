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



#ifndef _OFAPCH_HXX
#define _OFAPCH_HXX

#define _SVSTDARR_STRINGSISORTDTOR
#define _STD_VAR_ARRAYS
#define _INCL_EXPTRAY
#define _INCL_SFXNEWS

#ifndef OS2
// initialized data in header!
#include <sysdep.hxx>
#endif
#include <svgen.hxx>
#include <tlgen.hxx>
#include <sfx.hxx>
#include <sfxsh.hxx>
#include <sfxdoc.hxx>
#include <sfxdlg.hxx>
#include <sfxview.hxx>
#include <sfxiiter.hxx>
#include <svstdarr.hxx>
#include <svmem.hxx>
#define ITEMID_SPELLCHECK SID_ATTR_SPELL
#define ITEMID_HYPHENREGION 	SID_ATTR_HYPHENREGION
#ifndef OS2
#include <options.hxx>
#endif
namespace binfilter {

} //namespace binfilter
#endif
