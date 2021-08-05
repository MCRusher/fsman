#ifndef FILESYSTEM_MANIPULATION_H
#define FILESYSTEM_MANIPULATION_H

//#include <custom/os.h>
#define OS_LINUX 1

#include <stdbool.h>

#if OS_WINDOWS
struct fsiter {
    void * hfdata;
    char const * path;
};
#elif OS_LINUX
#include <dirent.h>
struct fsiter {
    DIR * dir;
    char const * path;
};
#else
#error fuck you
#endif

typedef struct fsiter fsiter;

typedef struct fsinfo {
    bool is_folder;
    bool can_read;
    bool can_write;
} fsinfo;

typedef enum fserror {
    fserror_FAILURE = false,
    fserror_SUCCESS = true,
    fserror_EXISTS,
} fserror;

bool fsinfo_get(fsinfo * info, char const * path);

bool fsman_copy(char const * from, char const * to);

bool fsman_hardLink(char const * from, char const * to);

bool fsman_softLink(char const * from, char const * to);

bool fsman_delete(char const * path, bool recursive);

fserror fsman_makeFolder(char const * path, bool create_subs);

fserror fsman_makeFile(char const * path);

size_t fsman_nameFromPath(char const * path);

bool fsman_exists(char const * path);

bool fsiter_init(fsiter * iter, char const * path);

bool fsiter_next(fsiter * iter);

void fsiter_deinit(fsiter * iter);

#endif //FILESYSTEM_MANIPULATION_H