/* -*- Mode: c; c-basic-offset: 2 -*-
 *
 * rasqal_ntriples.c - Rasqal N-Triples parsing utilities
 *
 * Copyright (C) 2013, David Beckett http://www.dajobe.org/
 *
 * This package is Free Software and part of Redland http://librdf.org/
 *
 * It is licensed under the following three licenses as alternatives:
 *   1. GNU Lesser General Public License (LGPL) V2.1 or any newer version
 *   2. GNU General Public License (GPL) V2 or any newer version
 *   3. Apache License, V2.0 or any newer version
 *
 * You may not use this file except in compliance with at least one of
 * the above three licenses.
 *
 * See LICENSE.html or LICENSE.txt at the top of this package for the
 * complete terms and further detail along with the license texts for
 * the licenses in COPYING.LIB, COPYING and LICENSE-2.0.txt respectively.
 *
 */


#ifdef HAVE_CONFIG_H
#include <rasqal_config.h>
#endif

#ifdef WIN32
#include <win32_rasqal_config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif


/* Rasqal includes */
#include "rasqal.h"
#include "rasqal_internal.h"


/* from raptor_uri.c */
#if RAPTOR_VERSION >= 20011

#else
static int
rasqal_uri_uri_string_is_absolute(const unsigned char* uri_string)
{
  const unsigned char* s = uri_string;
  
  /* 
   * scheme = alpha *( alpha | digit | "+" | "-" | "." )
   *    RFC 2396 section 3.1 Scheme Component
   */
  if(*s && isalpha((int)*s)) {
    s++;

    while(*s && (isalnum((int)*s) ||
                 (*s == '+') || (*s == '-') || (*s == '.')))
      s++;
  
    if(*s == ':')
      return 1;
  }


  return 0;
}

#define raptor_uri_uri_string_is_absolute rasqal_uri_uri_string_is_absolute

#endif


#if RAPTOR_VERSION >= 20012

#else

/**********************************************************************/

/** Internals imported from raptor */

/* turtle_common.c */
static int
rasqal_turtle_check_uri_string(unsigned char *string)
{
  unsigned char c;

  if(!string)
    return 0;
  
  while((c = *string++)) {
    if(((c <= 0x20) ||
        c == '<' || c == '>' || c == '"' || c == '{' || c == '}' || 
        c == '|' || c == '^' || c == '`' || c == '\\'))
      return 0;
  }
  return 1;
}

/* raptor_general.c */
static int
rasqal_check_ordinal(const unsigned char *name)
{
  int ordinal= -1;
  unsigned char c;

  while((c=*name++)) {
    if(c < '0' || c > '9')
      return -1;
    if(ordinal <0)
      ordinal = 0;
    ordinal *= 10;
    ordinal += (c - '0');
  }
  return ordinal;
}

/**********************************************************************/

/* raptor_ntriples.c converted to use above */

/* These are for 7-bit ASCII and not locale-specific */
#define IS_ASCII_ALPHA(c) (((c) > 0x40 && (c) < 0x5B) || ((c) > 0x60 && (c) < 0x7B))
#define IS_ASCII_UPPER(c) ((c) > 0x40 && (c) < 0x5B)
#define IS_ASCII_DIGIT(c) ((c) > 0x2F && (c) < 0x3A)
#define IS_ASCII_PRINT(c) ((c) > 0x1F && (c) < 0x7F)
#define TO_ASCII_LOWER(c) ((c)+0x20)

typedef enum {
  RASQAL_TERM_CLASS_URI,      /* ends on > */
  RASQAL_TERM_CLASS_BNODEID,  /* ends on first non [A-Za-z][A-Za-z0-9]* */
  RASQAL_TERM_CLASS_STRING,   /* ends on non-escaped " */
  RASQAL_TERM_CLASS_LANGUAGE  /* ends on first non [a-z0-9]+ ('-' [a-z0-9]+ )? */
} rasqal_ntriples_term_class;


static int
rasqal_ntriples_term_valid(unsigned char c, int position,
                           rasqal_ntriples_term_class term_class)
{
  int result = 0;

  switch(term_class) {
    case RASQAL_TERM_CLASS_URI:
      /* ends on > */
      result = (c != '>');
      break;

    case RASQAL_TERM_CLASS_BNODEID:
      /* ends on first non [A-Za-z0-9_:][-.A-Za-z0-9]* */
      result = IS_ASCII_ALPHA(c) || IS_ASCII_DIGIT(c) || c == '_' || c == ':';
      if(position)
        /* FIXME
         * This isn't correct; '.' is allowed in positions 1..N-1 but
         * this calling convention of character-by-character cannot
         * check this.
         */
        result = (result || c == '-' || c == '.');
      break;

    case RASQAL_TERM_CLASS_STRING:
      /* ends on " */
      result = (c != '"');
      break;

    case RASQAL_TERM_CLASS_LANGUAGE:
      /* ends on first non [a-zA-Z]+ ('-' [a-zA-Z0-9]+ )? */
      result = IS_ASCII_ALPHA(c);
      if(position)
        result = (result || IS_ASCII_DIGIT(c) || c == '-');
      break;

    default:
      RASQAL_DEBUG2("Unknown N-Triples term class %d", term_class);
  }

  return result;
}


/*
 * rasqal_ntriples_parse_term_internal:
 * @world: rasqal world
 * @locator: locator object (in/out) (or NULL)
 * @start: pointer to starting character of string (in)
 * @dest: destination of string (in)
 * @lenp: pointer to length of string (in/out)
 * @dest_lenp: pointer to length of destination string (out)
 * @end_char: string ending character
 * @class: string class
 *
 * INTERNAL - Parse an N-Triples term with escapes.
 *
 * Relies that @dest is long enough; it need only be as large as the
 * input string @start since when UTF-8 encoding, the escapes are
 * removed and the result is always less than or equal to length of
 * input.
 *
 * N-Triples strings / URIs are written in ASCII at present;
 * characters outside the printable ASCII range are discarded with a
 * warning.  See the grammar for full details of the allowed ranges.
 *
 * UTF-8 and the \u and \U esapes are both allowed.
 *
 * Return value: Non 0 on failure
 **/
static int
rasqal_ntriples_parse_term_internal(rasqal_world* world,
                                    raptor_locator* locator,
                                    const unsigned char **start,
                                    unsigned char *dest,
                                    size_t *lenp, size_t *dest_lenp,
                                    char end_char,
                                    rasqal_ntriples_term_class term_class)
{
  const unsigned char *p = *start;
  unsigned char c = '\0';
  size_t ulen = 0;
  unsigned long unichar = 0;
  unsigned int position = 0;
  int end_char_seen = 0;

  /* find end of string, fixing backslashed characters on the way */
  while(*lenp > 0) {
    int unichar_width;

    c = *p;

    p++;
    (*lenp)--;
    if(locator) {
      locator->column++;
      locator->byte++;
    }

    if(c > 0x7f) {
      /* just copy the UTF-8 bytes through */
      int unichar_len;
      unichar_len = raptor_unicode_utf8_string_get_char(p - 1, 1 + *lenp, NULL);
      if(unichar_len < 0 || RASQAL_GOOD_CAST(size_t, unichar_len) > *lenp) {
        rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "UTF-8 encoding error at character %d (0x%02X) found.", c, c);
        /* UTF-8 encoding had an error or ended in the middle of a string */
        return 1;
      }
      memmove(dest, p-1, unichar_len);
      dest += unichar_len;

      unichar_len--; /* p, *lenp were moved on by 1 earlier */

      p += unichar_len;
      (*lenp) -= unichar_len;
      if(locator) {
        locator->column += unichar_len;
        locator->byte += unichar_len;
      }
      continue;
    }

    if(c != '\\') {
      /* finish at non-backslashed end_char */
      if(end_char && c == end_char) {
        end_char_seen = 1;
        break;
      }

      if(!rasqal_ntriples_term_valid(c, position, term_class)) {
        if(end_char) {
          /* end char was expected, so finding an invalid thing is an error */
          rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Missing terminating '%c' (found '%c')", end_char, c);
          return 0;
        } else {
          /* it's the end - so rewind 1 to save next char */
          p--;
          (*lenp)++;
          if(locator) {
            locator->column--;
            locator->byte--;
          }
          if(term_class == RASQAL_TERM_CLASS_BNODEID && dest[-1] == '.') {
            /* If bnode id ended on '.' move back one */
            dest--;

            p--;
            (*lenp)++;
            if(locator) {
              locator->column--;
              locator->byte--;
            }
          }
          break;
        }
      }

      /* otherwise store and move on */
      *dest++ = c;
      position++;
      continue;
    }

    if(!*lenp) {
      rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "\\ at end of input.");
      return 0;
    }

    c = *p;

    p++;
    (*lenp)--;
    if(locator) {
      locator->column++;
      locator->byte++;
    }

    switch(c) {
      case '"':
      case '\\':
        *dest++ = c;
        break;
      case 'b':
        *dest++ = '\b';
        break;
      case 'f':
        *dest++ = '\f';
        break;
      case 'n':
        *dest++ = '\n';
        break;
      case 'r':
        *dest++ = '\r';
        break;
      case 't':
        *dest++ = '\t';
        break;
      case '<':
      case '>':
      case '{':
      case '}':
      case '|':
      case '^':
      case '`':
        /* Turtle 2013 allows these in URIs (as well as \" and \\) */
        *dest++ = c;
        break;

      case 'u':
      case 'U':
        ulen = (c == 'u') ? 4 : 8;

        if(*lenp < ulen) {
          rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "%c over end of input.", c);
          return 0;
        }

        if(1) {
          unsigned int ii;
          int n = 0;

          for(ii = 0; ii < ulen; ii++) {
            char cc = p[ii];
            if(!isxdigit(RASQAL_GOOD_CAST(char, cc))) {
              rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "N-Triples string error - illegal hex digit %c in Unicode escape '%c%s...'",
                            cc, c, p);
              n = 1;
              break;
            }
          }

          if(n)
            break;

          n = sscanf((const char*)p, ((ulen == 4) ? "%04lx" : "%08lx"), &unichar);
          if(n != 1) {
            rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Illegal Uncode escape '%c%s...'", c, p);
            break;
          }
        }

        p += ulen;
        (*lenp) -= ulen;
        if(locator) {
          locator->column += RASQAL_GOOD_CAST(int, ulen);
          locator->byte += RASQAL_GOOD_CAST(int, ulen);
        }

        if(unichar > rasqal_unicode_max_codepoint) {
          rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Illegal Unicode character with code point #x%lX (max #x%lX).", unichar, rasqal_unicode_max_codepoint);
          break;
        }

        unichar_width = raptor_unicode_utf8_string_put_char(unichar, dest, 4);
        if(unichar_width < 0) {
          rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Illegal Unicode character with code point #x%lX.", unichar);
          break;
        }

        /* The destination length is set here to 4 since we know that in
         * all cases, the UTF-8 encoded output sequence is always shorter
         * than the input sequence, and the buffer is edited in place.
         *   \uXXXX: 6 bytes input - UTF-8 max 3 bytes output
         *   \uXXXXXXXX: 10 bytes input - UTF-8 max 4 bytes output
         */
        dest += (int)unichar_width;
        break;

      default:
        rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Illegal string escape \\%c in \"%s\"", c, (char*)start);
        return 0;
    }

    position++;
  } /* end while */


  if(end_char && !end_char_seen) {
    rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Missing terminating '%c' before end of input.", end_char);
    return 1;
  }

  /* terminate dest, can be shorter than source */
  *dest = '\0';

  if(dest_lenp)
    *dest_lenp = p - *start;

  *start = p;

  return 0;
}


static int
rasqal_parse_turtle_term_internal(rasqal_world* world,
                                  raptor_locator* locator,
                                  const unsigned char **start,
                                  unsigned char *dest,
                                  size_t *len_p, size_t *dest_lenp,
                                  raptor_uri** datatype_uri_p)
{
  const unsigned char *p = *start;
  unsigned int position = 0;
  /* 0 = xsd:integer; 1= xsd:decimal; 2= xsd:double */
  short dtype = 0;
  int after_e = 0;

  while(*len_p > 0) {
    unsigned char c = *p;

    if(after_e) {
      if(!((c >= '0' && c <'9') || c == '+' || c == '-'))
        break;
      after_e = 0;
    } else if((position > 0 && (c == '+' || c == '-')) ||
       !((c >= '0' && c <'9') || c == '.' || c == 'e' || c == 'E'))
      break;

    if(c == '.')
      dtype = 1;
    else if(c == 'e' || c == 'E') {
      dtype = 2;
      after_e = 1;
    }

    p++;
    (*len_p)--;
    if(locator) {
      locator->column++;
      locator->byte++;
    }

    *dest++ = c;

    position++;
  }

  *dest = '\0';

  if(dest_lenp)
    *dest_lenp = p - *start;

  *start = p;

  if(dtype == 0)
    *datatype_uri_p = raptor_uri_copy(rasqal_xsd_datatype_type_to_uri(world, RASQAL_LITERAL_INTEGER));
  else if (dtype == 1)
    *datatype_uri_p = raptor_uri_copy(rasqal_xsd_datatype_type_to_uri(world, RASQAL_LITERAL_DECIMAL));
  else
    *datatype_uri_p = raptor_uri_copy(rasqal_xsd_datatype_type_to_uri(world, RASQAL_LITERAL_DOUBLE));

  return 0;
}


/*
 * rasqal_ntriples_parse_term:
 * @world: rasqal world
 * @locator: rasqal locator (in/out) (or NULL)
 * @string: string input (in)
 * @len_p: pointer to length of @string (in/out)
 * @term_p: pointer to store term (out)
 * @allow_turtle: non-0 to allow Turtle forms such as integers, boolean
 *
 * INTERNAL - Parse an N-Triples string into a #raptor_term
 *
 * The @len_p destination and @locator fields are modified as parsing
 * proceeds to be used in error messages.  The final value is written
 * into the #raptor_term pointed at by @term_p
 *
 * Return value: number of bytes processed or 0 on failure
 */
static size_t
rasqal_ntriples_parse_term(rasqal_world* world, raptor_locator* locator,
                           unsigned char *string, size_t *len_p,
                           raptor_term** term_p, int allow_turtle)
{
  unsigned char *p = string;
  unsigned char *dest;
  size_t term_length = 0;

  switch(*p) {
    case '<':
      dest = p;

      p++;
      (*len_p)--;
      if(locator) {
        locator->column++;
        locator->byte++;
      }

      if(rasqal_ntriples_parse_term_internal(world, locator,
                                             (const unsigned char**)&p,
                                             dest, len_p, &term_length,
                                             '>', RASQAL_TERM_CLASS_URI)) {
        goto fail;
      }

      if(!rasqal_turtle_check_uri_string(dest)) {
        rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "URI '%s' contains bad character(s)", dest);
        goto fail;
      }

      if(1) {
        raptor_uri *uri;

        /* Check for bad ordinal predicate */
        if(!strncmp((const char*)dest,
                    "http://www.w3.org/1999/02/22-rdf-syntax-ns#_", 44)) {
          int ordinal = rasqal_check_ordinal(dest + 44);
          if(ordinal <= 0)
            rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Illegal ordinal value %d in property '%s'.", ordinal, dest);
        }
        if(raptor_uri_uri_string_is_absolute(dest) <= 0) {
          rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "URI '%s' is not absolute.", dest);
          goto fail;
        }

        uri = raptor_new_uri(world->raptor_world_ptr, dest);
        if(!uri) {
          rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Could not create URI for '%s'", (const char *)dest);
          goto fail;
        }

        *term_p = raptor_new_term_from_uri(world->raptor_world_ptr, uri);
        raptor_free_uri(uri);
      }
      break;

    case '-':
    case '+':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if(allow_turtle) {
        raptor_uri* datatype_uri = NULL;

        dest = p;

        if(rasqal_parse_turtle_term_internal(world, locator,
                                             (const unsigned char**)&p,
                                             dest, len_p, &term_length,
                                             &datatype_uri)) {
          goto fail;
        }

        *term_p = raptor_new_term_from_literal(world->raptor_world_ptr,
                                               dest,
                                               datatype_uri,
                                               NULL /* language */);
      } else
        goto fail;
      break;

    case '"':
      dest = p;

      p++;
      (*len_p)--;
      if(locator) {
        locator->column++;
        locator->byte++;
      }

      if(rasqal_ntriples_parse_term_internal(world, locator,
                                             (const unsigned char**)&p,
                                             dest, len_p, &term_length,
                                             '"', RASQAL_TERM_CLASS_STRING)) {
        goto fail;
      }

      if(1) {
        unsigned char *object_literal_language = NULL;
        unsigned char *object_literal_datatype = NULL;
        raptor_uri* datatype_uri = NULL;

        if(*len_p && *p == '@') {
          unsigned char *q;
          size_t lang_len;

          object_literal_language = p;

          /* Skip - */
          p++;
          (*len_p)--;
          if(locator) {
            locator->column++;
            locator->byte++;
          }

          if(!*len_p) {
            rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Missing language after \"string\"-");
            goto fail;
          }

          if(rasqal_ntriples_parse_term_internal(world, locator,
                                  (const unsigned char**)&p,
                                  object_literal_language, len_p, &lang_len,
                                  '\0', RASQAL_TERM_CLASS_LANGUAGE)) {
            goto fail;
          }

          if(!lang_len) {
            rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Invalid language tag at @%s", p);
            goto fail;
          }

          /* Normalize language to lowercase
           * http://www.w3.org/TR/rdf-concepts/#dfn-language-identifier
           */
          for(q = object_literal_language; *q; q++) {
            if(IS_ASCII_UPPER(*q))
              *q = TO_ASCII_LOWER(*q);
          }

        }

        if(*len_p > 1 && *p == '^' && p[1] == '^') {

          object_literal_datatype = p;

          /* Skip ^^ */
          p += 2;
          *len_p -= 2;
          if(locator) {
            locator->column += 2;
            locator->byte += 2;
          }

          if(!*len_p || (*len_p && *p != '<')) {
            rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Missing datatype URI-ref in\"string\"^^<URI-ref> after ^^");
            goto fail;
          }

          p++;
          (*len_p)--;
          if(locator) {
            locator->column++;
            locator->byte++;
          }

          if(rasqal_ntriples_parse_term_internal(world, locator,
                                  (const unsigned char**)&p,
                                  object_literal_datatype, len_p, NULL,
                                  '>', RASQAL_TERM_CLASS_URI)) {
            goto fail;
          }

          if(raptor_uri_uri_string_is_absolute(object_literal_datatype) <= 0) {
            rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Datatype URI '%s' is not absolute.", object_literal_datatype);
            goto fail;
          }

        }

        if(object_literal_datatype && object_literal_language) {
          rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Typed literal used with a language - ignoring the language");
          object_literal_language = NULL;
        }

        if(object_literal_datatype) {
          datatype_uri = raptor_new_uri(world->raptor_world_ptr,
                                        object_literal_datatype);
          if(!datatype_uri) {
            rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Could not create literal datatype uri '%s'", object_literal_datatype);
            goto fail;
          }
          object_literal_language = NULL;
        }

        *term_p = raptor_new_term_from_literal(world->raptor_world_ptr,
                                               dest,
                                               datatype_uri,
                                               object_literal_language);
      }

      break;


      case '_':
        /* store where _ was */
        dest = p;

        p++;
        (*len_p)--;
        if(locator) {
          locator->column++;
          locator->byte++;
        }

        if(!*len_p || (*len_p > 0 && *p != ':')) {
          rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Illegal bNodeID - _ not followed by :");
          goto fail;
        }

        /* Found ':' - move on */

        p++;
        (*len_p)--;
        if(locator) {
          locator->column++;
          locator->byte++;
        }

        if(rasqal_ntriples_parse_term_internal(world, locator,
                                               (const unsigned char**)&p,
                                               dest, len_p, &term_length,
                                               '\0',
                                               RASQAL_TERM_CLASS_BNODEID)) {
          goto fail;
        }

        if(!term_length) {
          rasqal_log_error_simple(world, RAPTOR_LOG_LEVEL_ERROR, locator, "Bad or missing bNodeID after _:");
          goto fail;
        }

        *term_p = raptor_new_term_from_blank(world->raptor_world_ptr, dest);

        break;

      default:
        RASQAL_DEBUG2("Unknown term type '%c'", *p);
        goto fail;
    }

  fail:

  return p - string;
}


/*
 * rasqal_new_term_from_counted_string:
 * @world: rasqal world
 * @string: N-Triples format string (UTF-8)
 * @length: length of @string (or 0)
 *
 * INTERNAL - create a new term from an N-Triples format string in UTF-8
 *
 * Return value: new term or NULL on failure
*/
static raptor_term*
rasqal_new_term_from_counted_string(rasqal_world* world,
                                    unsigned char* string, size_t length)
{
  raptor_term* term = NULL;
  size_t bytes_read;
  raptor_locator locator;

  if(!string)
    return NULL;

  if(!length)
    length = strlen(RASQAL_GOOD_CAST(const char*, string));

  memset(&locator, sizeof(locator), '\0');
  locator.line = -1;

  bytes_read = rasqal_ntriples_parse_term(world, &locator,
                                          string, &length, &term, 1);

  if(!bytes_read || length != 0) {
    if(term)
      raptor_free_term(term);
    term = NULL;
  }

  return term;
}

#endif


/*
 * rasqal_new_literal_from_ntriples_counted_string:
 * @world: rasqal world
 * @string: N-Triples format string (UTF-8)
 * @length: length of @string (or 0)
 *
 * INTERNAL - create a new literal from an N-Triples format string in UTF-8
 *
 * Return value: new literal or NULL on failure
*/
rasqal_literal*
rasqal_new_literal_from_ntriples_counted_string(rasqal_world* world,
                                                unsigned char* string,
                                                size_t length)
{
  raptor_term* term;
  rasqal_literal* l;

#if RAPTOR_VERSION >= 20012
  term = raptor_new_term_from_counted_string(world->raptor_world_ptr,
                                             string, length);
#else
  term = rasqal_new_term_from_counted_string(world, string, length);
#endif

  if(!term)
    return NULL;

  l = rasqal_new_literal_from_term(world, term);

  raptor_free_term(term);

  return l;
}
