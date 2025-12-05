#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

char separators[] = " -\r\t\n./,";

int is_separators(char c){
    return strchr(separators, c) != 0;
}

int
main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: sixfive <filename>...\n");
        exit(1);
    }
    for (int i = 1; i < argc; ++i)
    {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0)
        {
            fprintf(2, "sixfive: cannot open %s\n", argv[i]);
            continue;
        }

        char buf[1];
        char numbuf[128];
        int numlen = 0;
        while (read(fd, buf, 1) == 1)
        {
            char c = buf[0];
            if (c >= '0' && c <= '9')
            {
                if (numlen < sizeof(numbuf) - 1)
                    numbuf[numlen++] = c;
            }
            else if (is_separators(c))
            {
                if (numlen > 0)
                {
                    numbuf[numlen] = '\0';
                    int n = atoi(numbuf);
                    if (n % 5 == 0 || n % 6 == 0)
                        printf("%d\n", n);
                    numlen = 0;
                }
            }
            else
            {
                numlen = 0;
            }
        }
        if (numlen > 0)
        {
            numbuf[numlen] = '\0';
            int n = atoi(numbuf);
            if (n % 5 == 0 || n % 6 == 0)
                printf("%d\n", n);
        }
        close(fd);
    }
    
    exit(0);

}