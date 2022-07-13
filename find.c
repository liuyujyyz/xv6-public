#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char * dot = ".             ";
char * dot2 = "..            ";

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

int 
vanilla_find(char * path, char * fname, char * res)
{
    char buf[512], *p, *cur_name;
    char a[14], b[14];
    int fd, tmp;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) return -1;
    if (fstat(fd, &st) < 0) 
    {
        close(fd);
        return -1;
    }
    if (st.type == T_FILE){
        close(fd);
        strcpy(a, fmtname(fname));
        strcpy(b, fmtname(path));
        tmp = strcmp(a, b);
        if (tmp == 0){
            strcpy(res, path);
            return 0;
        }else return -1;
    }
    if (st.type == T_DIR){
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            cur_name = fmtname(buf);
            if ((strcmp(cur_name, dot) != 0) && (strcmp(cur_name, dot2) != 0)){
                tmp = vanilla_find(buf, fname, res);
                if (tmp == 0) return 0;
            }           
        }
        close(fd);
        return -1;
    }
    close(fd);
    return -1;
}

void
find(char *path, char * fname)
{
  char buf[512];
  struct stat st;

  int res = vanilla_find(path, fname, buf);
  if (res == 0){
      stat(buf, &st);
      printf(1, "\e2%s\e7 %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
  }else printf(1, "\e1can not find %s\n", fname);
}

int
main(int argc, char *argv[])
{
  if (argc < 3){
    printf(1, "no enough args\n");
    exit();
  }
  find(argv[1], argv[2]);
  exit();
}
