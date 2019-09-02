#ifndef MYHASH_H
#define MYHASH_H

#include "mystring.h"
#include <string.h> //
#include <glib.h> // 


/* fucking hell, glib */

static
guint
mystring_hash (gconstpointer v)
{
  const string s = *((const string*)v);
  int o;
  guint32 h = 5381;

  for (o = 0; o != s.len; ++o) {
    h = (h << 5) + h + s.base[o];
  }

  return h;
}

static
gboolean
mystring_equal (gconstpointer v1,
             gconstpointer v2)
{
  const string string1 = *((const string*)v1);
  const string string2 = *((const string*)v2);

  return string1.len == string2.len &&
    (0 == memcmp(string1.base,string2.base,string1.len));
}

#endif /* MYHASH_H */
