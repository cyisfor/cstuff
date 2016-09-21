#include <sys/stat.h>
#include <glib.h>

typedef struct path *path;

// append gchar* components to path
path path_add(path self, ...);

// look up a base path
path path_lookup(GUserDirectory id);
path path_home();

const gchar* path_base(path);
gsize path_len(path);

void path_free(path);

bool left_is_older(path left, path right);
void path_check_terminate(path);
