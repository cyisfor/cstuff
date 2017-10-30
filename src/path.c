#include "path.h"

#include <string.h> // glib has no memcpy

typedef struct path {
	const gchar* base;
	struct stat info;
	gsize len;
	gsize allocated_len;
	bool constant;
	bool statted;
	bool exists;
} *path;

path path_new(gchar* s, gsize len) {
	path self = g_new(struct path,1);
	self->base = s;
	self->len = len;
	self->allocated_len = len;
	self->constant = false;
}

#define ORDER 8
#define BLOCK (1<<ORDER)

static path prepare_append(path self, gsize length) {
	if(len == 0) return;
	if(self->constant) {
		path old = self;
		self = g_new(struct path,1);
		self->constant = false;
		self->len = old->len;
		self->allocated_len = ((self->len + length) >> ORDER + 1) << ORDER;
		self->base = g_new(gchar,self->allocated_len);
		g_memcpy(self->base,old->base,self->len);
		// don't self->len += length yet
	} else {
		if(self->allocated_len < self->len + length) {
			self->allocated_len = ((self->len + length) >> ORDER + 1) << ORDER;
			self->base = g_realloc(self->base, self->allocated_len);
		}
	}
	return self;
}

static void appendslow(path self, const gchar* s, gsize length) {
	/* since the tail of base is never used, there's never a trailing
		 suffix. Either the string overlaps from before base, it
		 totally overlaps, or it doesn't overlap. */
	if(G_UNLIKELY(s >= self->base)) {
		if(s < self->base + self->len) {
			// totally overlapping
			g_memmove(self->base+self->len,s,length);
		} else {
			g_assert(s + length < self->base + self->len);
			g_memcpy(self->base+self->len,s,length);
		}
	} else {
		if(s + length > self->base) {
			// overlapping suffix
			gsize pivot = self->base - s;
			g_memcpy(self->base+self->len,s,pivot);
			g_memmove(self->base+self->len+pivot,s+pivot,length-pivot);
		} else {
			g_memcpy(self->base+self->len,s,length);
		}
	}
	self->len += len;
}

// for known consts, like separators and such.
static void appendfast(path self, const gchar* s, gsize length) {
	switch(length) {
	case 1:
		self->base[self->len++] = s[0];
		break;
	case 2:
		self->base[self->len] = s[0];
		self->base[self->len+1] = s[1];
		len += 2;
		break;
	case 3:
		self->base[self->len] = s[0];
		self->base[self->len+1] = s[1];
		self->base[self->len+2] = s[2];
		len += 3;
		break;
	default:
		g_memcpy(self->base+self->len, s, length);
		self->len += len;
		break;
	};
}

path path_add1(path self, gchar* name) {
	gsize len = g_strlen(name);
	self = prepare_append(self, len+sizeof(G_DIR_SEPARATOR_S)-1);
	appendfast(self,G_DIR_SEPARATOR_S,sizeof(G_DIR_SEPARATOR_S)-1);
	appendslow(self,name,len);
	return self;
}

path path_add(path self, ...) {
	va_list arg;
	va_start(arg, self);
	if(self == NULL) {
		self = CWD();
	}
	for(;;) {
		gchar* next = va_arg(arg, gchar*);
		if(next == NULL) break;
		self = path_add1(self, name);
	}
	return self;
}

path path_lookup(GUserDirectory id) {
	static struct path paths[G_USER_N_DIRECTORIES];
	static bool gotchas[G_USER_N_DIRECTORIES] = {};
	if(!gotchas[id]) {
		const gchar* s = g_get_user_special_dir(id);
		paths[id].base = s;
		paths[id].len = strlen(s);
		paths[id].constant = true;
		gotchas[id] = true;
	}

	return &paths[id];
}

/* we can't use g_stat because it doesn't use timespec modification times */

struct stat path_stat(path self) {
	if(!self->statted) {
		self->statted = true;
		self->exists = (0==stat(self->base,&self->info));
		if(!self->exists) {
			g_assert(errno == EEXIST);
		}
	}
	return self->info;
}	

path path_home(void) {
	static struct path home;
	bool gotcha = false;
	if(!gotcha) {
		const gchar* s = g_getenv("HOME");
		if(s == NULL) {
			s = g_get_home_dir();
		} else {
			if(g_path_is_absolute(s)  && (0 == g_stat(s,&home.info))) {
				home.statted = true;
			} else {
				// to hell with "undefined behavior"
				g_warning("HOME is set to something bad %s",s);
				const gchar* old = s;
				g_unsetenv("HOME");
				s = g_get_home_dir();
				g_setenv("HOME",old);
			}
		}
		home.base = s;
		home.len = g_strlen(s);
		home.constant = true;
		gotcha = true;
	}
	return &home;
}

const gchar* path_base(path self) {
	return self->base;
}

gsize path_len(path self) {
	return self->len;
}

void path_free(path self) {
	if(self->constant) return;
	free(self->base);
	free(self);
}

bool left_is_older(path left, path right) {
	if(left->info.st_mtime < right->info.st_mtime) return true;
	if(left->info.st_mtime == right->info.st_mtime)
		if(left->info.st_mtim.tv_nsec < right->info.st_mtim.tv_nsec)
			return true;
	return false;
}

void path_check_terminate(path self) {
	char* match = getenv("TARGET");
	if(match == NULL) return;
	if(0==strcmp(self->base,match)) {
		printf("reached target %s (%s)\n",target,match);
		exit(0);
	}
}

path CWD(void) {
	static struct path self = {
		.base = ".",
		.len = 1
		.constant = true;
	};
	return &self;
}

bool path_exists(path self) {
	return self->exists;
}

const char* path_add_ext(const char* name, const char* ext) {
	static char buf[0x100];
	ssize_t len = strlen(name);
	ssize_t elen = strlen(ext);
	assert(len + elen + 2 < 0x100);
	memcpy(buf,name,len);
	buf[len] = '.';
	memcpy(buf+len+1,ext,elen);
	buf[len+1+elen] = '\0';
	return buf;
}
