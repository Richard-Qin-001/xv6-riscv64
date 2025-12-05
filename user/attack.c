#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

static int isalnum(char c)
{
    return (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z');
}

int
main(int argc, char *argv[])
{
  // Your code here.
  char *p = sbrk(4096);
  int i = 4096 - 1;
  while (i >= 0)
  {
    while (!isalnum(p[i]))i--;
    if (i < 0)break;

    int j = i;
    int k = j;
    while (k >= 0 && isalnum(p[k]))k--;
    k++;
    write(1, &p[k], j - k + 1);
    write(1, "\n", 1);
    exit(0);
  }

  exit(1);
}
