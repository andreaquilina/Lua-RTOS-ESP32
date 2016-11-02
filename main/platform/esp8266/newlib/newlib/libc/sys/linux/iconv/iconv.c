/* Convert characters in input buffer using conversion descriptor to
   output buffer.
   Copyright (C) 1997, 1998, 1999, 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1997.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <stddef.h> /* for NULL */
#include <errno.h>
#include <iconv.h>

#include <gconv_int.h>

#include <assert.h>


size_t
iconv (iconv_t cd, char **__restrict inbuf,
       size_t *__restrict inbytesleft,
       char **__restrict outbuf,
       size_t *__restrict outbytesleft)
{
  __gconv_t gcd = (__gconv_t) cd;
  char *outstart = outbuf ? *outbuf : NULL;
  size_t irreversible;
  int result;

  if (__builtin_expect (inbuf == NULL || *inbuf == NULL, 0))
    {
      if (outbuf == NULL || *outbuf == NULL)
	result = __gconv (gcd, NULL, NULL, NULL, NULL, &irreversible);
      else
	result = __gconv (gcd, NULL, NULL, (unsigned char **) outbuf,
			  (unsigned char *) (outstart + *outbytesleft),
			  &irreversible);
    }
  else
    {
      const char *instart = *inbuf;

      result = __gconv (gcd, (const unsigned char **) inbuf,
			(const unsigned char *)  (*inbuf + *inbytesleft),
			(unsigned char **) outbuf,
			(unsigned char *) (*outbuf + *outbytesleft),
			&irreversible);

      *inbytesleft -= *inbuf - instart;
    }
  if (outstart != NULL)
    *outbytesleft -= *outbuf - outstart;

  switch (__builtin_expect (result, __GCONV_OK))
    {
    case __GCONV_ILLEGAL_DESCRIPTOR:
      __set_errno (EBADF);
      irreversible = (size_t) -1L;
      break;

    case __GCONV_ILLEGAL_INPUT:
      __set_errno (EILSEQ);
      irreversible = (size_t) -1L;
      break;

    case __GCONV_FULL_OUTPUT:
      __set_errno (E2BIG);
      irreversible = (size_t) -1L;
      break;

    case __GCONV_INCOMPLETE_INPUT:
      __set_errno (EINVAL);
      irreversible = (size_t) -1L;
      break;

    case __GCONV_EMPTY_INPUT:
    case __GCONV_OK:
      /* Nothing.  */
      break;

    default:
      assert (!"Nothing like this should happen");
    }

  return irreversible;
}