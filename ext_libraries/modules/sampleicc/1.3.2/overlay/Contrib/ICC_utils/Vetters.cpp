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
// -Initial implementation by Joseph Goldstone sumer 2007
//
//////////////////////////////////////////////////////////////////////

#include "Vetters.h"

#include <sstream>
using namespace std;

#ifndef WIN32
#ifdef sun
#include <errno.h>
#else
#include <sys/errno.h>
#endif
#else
#include <string.h>
#define stat _stat
#endif

#if defined WIN32 || defined sun
int strerror_r(int errnum, char *str, int strsize)
{
  const char *errstr = strerror(errnum);

  if (errstr) {
    strncpy(str, errstr, strsize);
    return 0;
  }

  return -1;
}
#endif

#include "ICC_tool_exception.h"

const char*
path_tail(const char* const s)
{
  const char* tail = strdup(s);
  const char* last_slash = strrchr(tail, '/');
  if (last_slash != NULL)
    tail = last_slash + 1;
  return tail;
}
    
void
vet_as_int(const char* const s, const string& name,
           const string& description)
{
  istringstream ss(s);
  int i;
  ss >> i;
  if (ss.fail())
  {
    ostringstream oss;
    oss << "The " << name << " argument given, `" << s << "', cannot be parsed as"
      << " an integer.  It should be an integer representing "
      << description << ".";
    throw ICC_tool_exception(oss.str());
  }
}

void
vet_as_float(const char* const s, const string& name,
             const string& description)
{
  istringstream ss(s);
  float i;
  ss >> i;
  if (ss.fail())
  {
    ostringstream oss;
    oss << "The " << name << " argument given, `" << s << "', cannot be parsed as"
      << " a floating-point number.  It should be a floating-point number"
      << " representing " << description << ".";
    throw ICC_tool_exception(oss.str());
  }
}

#define STRERROR_BUF_SIZE 256
off_t
get_size(const char* const s)
{ 
  struct stat sb;
  int stat_returned = stat(s, &sb);
  if (stat_returned < 0)
  {
    int stat_errno = errno;
    char strerror_buf[STRERROR_BUF_SIZE];
    strerror_r(stat_errno, strerror_buf, STRERROR_BUF_SIZE);
    ostringstream oss;
    oss << "Could not access information for file `" << s << "': "
      << strerror_buf;
    throw ICC_tool_exception(oss.str());
  }
  return sb.st_size;
}

bool
check_mode(const char* const s, mode_t mode)
{ 
  struct stat sb;
  int stat_returned = stat(s, &sb);
  if (stat_returned < 0)
  {
    int stat_errno = errno;
    char strerror_buf[STRERROR_BUF_SIZE];
    strerror_r(stat_errno, strerror_buf, STRERROR_BUF_SIZE);
    ostringstream oss;
    oss << "Could not access information for file `" << s << "': "
      << strerror_buf;
    throw ICC_tool_exception(oss.str());
  }
  return (sb.st_mode & mode) != 0;
}

bool
is_existent_file_pathname(const char* const s)
{
  struct stat sb;
  return stat(s, &sb) == 0;
}

bool
is_plain_file_pathname(const char* const s)
{
  return check_mode(s, S_IFREG);
}

bool
is_directory(const char* const s)
{
  return check_mode(s, S_IFDIR);
}

const char* const
containing_directory(const char* const s)
{
  if (strlen(s) == 0)
    throw ICC_tool_exception("name of directory passed to containing_directory"
                             " function was zero-length.");
  char* tmp = strdup(s);
  // lop off any trailing seperator
  if (tmp[strlen(tmp) - 1] == '/')
    tmp[strlen(tmp) - 1] = 0;
  if (strlen(tmp) == 0)
    throw ICC_tool_exception("root directory (which has no containing"
                             " directory) passed to containing_directory"
                             " function");
  char* idx = strrchr(tmp, '/');
  if (idx != NULL)
  {
    *idx = 0;
    return tmp;
  }
  char* current_directory = getenv("PWD");
  if (current_directory == NULL)
    throw ICC_tool_exception("pathname passed to containing_directory has no"
                             " embedded seperator, and there is no value for"
                             " PWD defined in the environment");
  return strdup(current_directory);
}

bool
is_readable_pathname(const char* const s)
{
  return check_mode(s, S_IRUSR)
  || check_mode(s, S_IRGRP)
  || check_mode(s, S_IROTH);
}

bool
is_writable_pathname(const char* const s)
{
  return check_mode(s, S_IWUSR)
  || check_mode(s, S_IWGRP)
  || check_mode(s, S_IWOTH);
}

bool
is_pathname_of_empty_file(const char* const s)
{
  return get_size(s) > 0;
}

void
vet_input_file_pathname(const char* const s, const string& name,
                        const string& description)
{
  if (! is_plain_file_pathname(s))
  {
    ostringstream oss;
    oss << "The " << name << " argument given, `" << s << "', is not the pathname"
      << " of a plain file (i.e. it is the pathname of a directory, or of a"
      << " symbolic link, or of some other sort of special file.)  It should be"
      << " " << description << ".";
    throw ICC_tool_exception(oss.str());
  }
  if (! is_readable_pathname(s))
  {
    ostringstream oss;
    oss << "The " << name << " argument given, `" << s << "', is not the pathname"
      << " of a readable file (i.e. you do not have permission to read that"
      << " file, or you do not have permission to read some directory"
      << " containing that file.";
      throw ICC_tool_exception(oss.str());
  }
  if (! is_pathname_of_empty_file(s))
  {
    ostringstream oss;
    oss << "The " << name << " argument given, `" << s << "', is not the pathname"
      << " of an existing readable file, but that file is of zero length."
      << " The argument should be " << description << ".";
    throw ICC_tool_exception(oss.str());
  }
}

void
vet_output_file_pathname(const char* const s, const string& name,
                         const string& description,
                         bool silent_overwrite_OK)
{
  const char* const container = containing_directory(s);
  if (! is_writable_pathname(container))
  {
    ostringstream oss;
    oss << "The " << name << " argument given, `" << s << "', has a directory"
      << " component which is not writable."
      << " The argument should be " << description << ".";
    throw ICC_tool_exception(oss.str());
  }
  if (is_existent_file_pathname(s))
  {
    if (is_plain_file_pathname(s))
      if (is_writable_pathname(s))
      {
        if (! silent_overwrite_OK)
        {
          ostringstream oss;
          oss << "The " << name << " argument given, `" << s << "' is of an existing"
            << " file."
            << " The argument should be " << description << ".";
          throw ICC_tool_exception(oss.str());
        }
      } else {
        ostringstream oss;
        oss << "The " << name << " argument given, `" << s << "' is of an existing"
          << " file which is not writable."
          << " The argument should be " << description << ".";
        throw ICC_tool_exception(oss.str());
      }
  }
}
