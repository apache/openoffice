/*
 File:       Vetters.cpp
 
 Contains:   Utility functions to handle common argument-checking tasks, in a 
             way that hides platform-specific details from higher-level code.
 
 Version:    V1
 
 Copyright:  © see below
 */

/*
 * The ICC Software License, Version 0.1
 *
 *
 * Copyright (c) 2003-2006 The International Color Consortium. All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        The International Color Consortium (www.color.org)"
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "ICC" and "The International Color Consortium" must
 *    not be used to imply that the ICC organization endorses or
 *    promotes products derived from this software without prior
 *    written permission. For written permission, please see
 *    <http://www.color.org/>.
 *
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNATIONAL COLOR CONSORTIUM OR
 * ITS CONTRIBUTING MEMBERS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the The International Color Consortium. 
 *
 *
 * Membership in the ICC is encouraged when this software is used for
 * commercial purposes. 
 *
 *  
 * For more information on The International Color Consortium, please
 * see <http://www.color.org/>.
 *  
 * 
 */

////////////////////////////////////////////////////////////////////// 
// HISTORY:
//
// -Initial implementation by Joseph Goldstone summer 2007
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEFINED_PATHNAME_H__
#define __DEFINED_PATHNAME_H__

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#ifdef WIN32
typedef unsigned short mode_t;
#define S_IRUSR S_IREAD
#define S_IRGRP S_IREAD
#define S_IROTH S_IREAD
#define S_IWUSR S_IWRITE
#define S_IWGRP S_IWRITE
#define S_IWOTH S_IWRITE
#endif

const char*
path_tail(const char* const s);

void
vet_as_int(const char* const s, const std::string& name,
           const std::string& description);

void
vet_as_float(const char* const s, const std::string& name,
             const std::string& description);

off_t
get_size(const char* const s);

bool
check_mode(const char* const s, mode_t mode);

bool
is_plain_file_pathname(const char* const s);

bool
is_readable_pathname(const char* const s);

bool
is_pathname_of_empty_file(const char* const s);

void
vet_input_file_pathname(const char* const s, const std::string& name,
                        const std::string& description);

void
vet_output_file_pathname(const char* const s, const std::string& name,
                         const std::string& description,
                         bool silent_overwrite_OK = false);
#endif
