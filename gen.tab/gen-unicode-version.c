/* FriBidi
 * gen-unicode-version.c - generate fribidi-unicode-version.h
 *
 * $Id: gen-unicode-version.c,v 1.11 2005-11-03 01:39:01 behdad Exp $
 * $Author: behdad $
 * $Date: 2005-11-03 01:39:01 $
 * $Revision: 1.11 $
 * $Source: /home/behdad/src/fdo/fribidi/togit/git/../fribidi/fribidi2/gen.tab/gen-unicode-version.c,v $
 *
 * Author:
 *   Behdad Esfahbod, 2001, 2002, 2004
 *
 * Copyright (C) 2004 Sharif FarsiWeb, Inc
 * Copyright (C) 2004 Behdad Esfahbod
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library, in a file named COPYING; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA
 * 
 * For licensing issues, contact <license@farsiweb.info>.
 */

#include <common.h>

#include <stdio.h>
#if STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# if HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#if HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif
#if HAVE_STRINGS_H
# include <strings.h>
#endif

#include "packtab.h"

#define appname "gen-unicode-version"
#define outputname "fribidi-unicode-version.h"

static void
die (
  const char *msg
)
{
  fprintf (stderr, appname ": %s\n", msg);
  exit (1);
}

static void
die2 (
  const char *fmt,
  const char *p
)
{
  fprintf (stderr, appname ": ");
  fprintf (stderr, fmt, p);
  fprintf (stderr, "\n");
  exit (1);
}

int version_major, version_minor, version_micro;
char unicode_version[100];
char buf[4000];

static void
init (
  void
)
{
  version_major = version_minor = version_micro = 0;
  strcpy (unicode_version, "(unknown)");
}

#define READ_VERSION(prefix) ((where = strstr(buf, prefix)) && \
			      (3 == sscanf (where, \
					    prefix"%d.%d.%d", &version_major, &version_minor, &version_micro)))

static int
read_file (
  FILE *f
)
{
  int found = 0;

  version_micro = 0;

  while (fgets (buf, sizeof buf, f))
    {
      const char *where;

      if (READ_VERSION ("Version ") || READ_VERSION ("Unicode "))
	{
	  found = 1;
	  break;
	}
    }

  if (!found)
    return 0;

  sprintf (unicode_version, "%d.%d.%d", version_major, version_minor,
	   version_micro);
  return 1;
}

static int
read_data (
  const char *data_file_type,
  const char *data_file_name
)
{
  FILE *f;
  int status;

  fprintf (stderr, "Reading `%s'\n", data_file_name);
  if (!(f = fopen (data_file_name, "rt")))
    die2 ("error: cannot open `%s' for reading", data_file_name);

  status = read_file (f);

  fclose (f);

  return status;
}

static void
gen_unicode_version (
  const char *data_file_type
)
{
  fprintf (stderr, "Generating `" outputname "'\n");
  printf ("/* " outputname "\n * generated by " appname " (" FRIBIDI_NAME " "
	  FRIBIDI_VERSION ")\n" " * from the file %s */\n\n", data_file_type);

  printf ("#define FRIBIDI_UNICODE_VERSION \"%s\"\n"
	  "#define FRIBIDI_UNICODE_MAJOR_VERSION %d\n"
	  "#define FRIBIDI_UNICODE_MINOR_VERSION %d\n"
	  "#define FRIBIDI_UNICODE_MICRO_VERSION %d\n"
	  "#define FRIBIDI_UNICODE_NAMESPACE(SYMBOL) \\\n"
	  "	FRIBIDI_NAMESPACE(SYMBOL##_unicode_%d_%d_%d)\n"
	  "#define FRIBIDI_UNICODE_PRIVATESPACE(SYMBOL) \\\n"
	  "	FRIBIDI_PRIVATESPACE(SYMBOL##_unicode_%d_%d_%d)\n"
	  "\n",
	  unicode_version,
	  version_major, version_minor, version_micro,
	  version_major, version_minor, version_micro,
	  version_major, version_minor, version_micro);

  printf ("/* End of generated " outputname " */\n");
}

int
main (
  int argc,
  const char **argv
)
{
  const char *data_file_type;
  int i;
  int found = 0;

  if (argc < 2)
    die ("usage:\n  " appname
	 " /path/to/a/UCD/file [/path/to/more/UCD/files ...]");

  {
    const char *data_file_name;

    init ();
    for (i = 1; i < argc && !found; i++)
      {
	data_file_name = argv[i];
	data_file_type = strrchr (data_file_name, '/');
	if (data_file_type)
	  data_file_type++;
	else
	  data_file_type = data_file_name;
	found = read_data (data_file_type, data_file_name);
      }
    if (!found)
      die ("error: version not found");
    gen_unicode_version (data_file_type);
  }

  return 0;
}
