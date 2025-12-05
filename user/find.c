#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

static char *
basename(char *path)
{
    char *p;
    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    return p + 1;
}

void
exec_in_find(int cmdc, char **cmdv, char *filepath)
{
    char *argv[MAXARG];
    if (cmdc + 1 >= MAXARG)
    {
        fprintf(2, "find: command too long\n");
        return;
    }
    for (int i = 0; i < cmdc; i++)
        argv[i] = cmdv[i];
    argv[cmdc] = filepath;
    argv[cmdc + 1] = 0;
    if (fork() == 0)
    {
        exec(argv[0], argv);
        exit(1);
    }
    wait(0);
}

void
find(char *path, char *target, int cmdc, char **cmdv)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type)
    {
        case T_DEVICE:
        case T_FILE:
            if(strcmp(basename(path), target) == 0)
            {
                if (cmdv) exec_in_find(cmdc, cmdv, path);
                else printf("%s\n", path);
            } 
            break;
        
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
            {
                fprintf(2, "find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';

            while(read(fd, &de, sizeof(de)) == sizeof de)
            {
                if (de.inum == 0)
                    continue;
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;

                find(buf, target, cmdc, cmdv);
            }
            break;
    }
    close(fd);
}

int
main(int argc, char * argv[])
{
    if(argc < 3)
    {
        fprintf(2, "Usage: find <path> <filename> [-exec <command>]\n");
        exit(1);
    }
    int exec_index = -1;
    for (int i = 3; i < argc; ++i)
    {
        if (strcmp(argv[i], "-exec") == 0)
        {
            exec_index = i;
            break;
        }
    }
    char **cmdv = 0;
    int cmdc = 0;
    if (exec_index != -1)
    {
        cmdv = &argv[exec_index + 1];
        cmdc = argc - exec_index - 1;
    }
    find(argv[1], argv[2], cmdc, cmdv);
    exit(0);
}