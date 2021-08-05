#include "fsman.h"

#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

bool fsinfo_get(fsinfo * info, char const * path) {
    struct stat buf;
    if(stat(path, &buf) != 0){
        return false;
    }
    info->is_folder = S_ISDIR(buf.st_mode);
    info->can_read = buf.st_mode & S_IRUSR;
    info->can_write = buf.st_mode & S_IWUSR;
    
    return true;
}

bool fsman_copy(char const * from, char const * to) {
    int in = open(from, O_RDONLY);
    if(in == -1){
        return  false;
    }
    int out = open(to, O_WRONLY);
    if(out == -1){
        close(in);
        return false;
    }
    
    char buf[8192];
    ssize_t res;
    do{
        res = read(in, buf, sizeof(buf));
        write(out, buf, res);
    }while(res != 0);
    return true;
}

bool fsman_hardLink(char const * from, char const * to) {
  return link(from, to) == 0;
}

bool fsman_softLink(char const * from, char const * to) {
  return symlink(from, to) == 0;
}

int fsman_delete(char const * path, bool recursive) {
  fsinfo info;
  if(!fsinfo_get(&info, path)){
    return 1;
  }
  if(info.is_folder){
    if(recursive){
      fsiter iter;
      if(!fsiter_init(&iter, path)){
        return 2;
      }
      while(iter.path != NULL){
        if(!fsman_delete(iter.path, true)){
          return 3;
        }
        if(!fsiter_next(&iter)){
          return false;
        }
      }
    }
    return rmdir(path) == 0;
  }else{
    return unlink(path) == 0;
  }
}

fserror fsman_makeFolder(char const * path, bool create_subs) {
  if(path[0] == '\0'){
    return false;
  }
  
  if(create_subs){
    size_t plen = strlen(path);
    char * mpath = malloc(plen + 1);
    if(mpath == NULL){
      return false;
    }
    strcpy(mpath, path);
    if(mpath[plen - 1] == '/'){
      --plen;
    }
    for(size_t i = 0; i < plen; i++){
      if(mpath[i] == '/'){
        mpath[i] = '\0';
        errno = 0;
        if(mkdir(mpath, S_IRWXU) != 0 && errno != EEXIST){
          return false;
        }
        mpath[i] = '/';
      }
    }
  }
  errno = 0;
  if(mkdir(path, S_IRWXU) != 0){
    if(errno != EEXIST){
      return fserror_FAILURE;
    }else{
      return fserror_EXISTS;
    }
  }
  return fserror_SUCCESS;
}

fserror fsman_makeFile(char const * path) {
  errno = 0;
  int fd = open(path, O_CREAT | O_EXCL);
  if(fd == -1){
    if(errno != EEXIST){
      return fserror_FAILURE;
    }else{
      return fserror_EXISTS;
    }
  }
  close(fd);
  return fserror_SUCCESS;
}

size_t fsman_nameFromPath(char const * path) {
  size_t pos = strlen(path) - 1;
  if(path[pos] == '/'){
    --pos;
  }
  while(path[pos] != '/'){
    --pos;
  }
  return pos + 1;
}

fserror fsman_doesNotExist(char const * path) {
  errno = 0;
  if(access(path, F_OK) == -1){
    if(errno != ENOENT){
      return fserror_FAILURE;
    }else{
      return fserror_SUCCESS;
    }
  }
  return fserror_EXISTS;
}

bool fsiter_init(fsiter * iter, char const * path) {
    iter->dir = opendir(path);
    if(iter->dir == NULL){
        return false;
    }
    errno = 0;
    struct dirent * pent = readdir(iter->dir);
    if(errno != 0){
        closedir(iter->dir);
        return false;
    }else if(pent != NULL){
        iter->path = pent->d_name;
    }else{
        iter->path = NULL;
    }
    
    return true;
}

bool fsiter_next(fsiter * iter) {
    errno = 0;
    struct dirent * pent = readdir(iter->dir);
    if(errno != 0){
        closedir(iter->dir);
        return false;
    }else if(pent != NULL){
        iter->path = pent->d_name;
    }else{
        iter->path = NULL;
    }

    return true;
}

void fsiter_deinit(fsiter * iter){
    closedir(iter->dir);
    iter->path = NULL;
}