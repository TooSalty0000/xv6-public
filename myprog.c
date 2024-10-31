#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
  // printf(1, "This is parent\n");
  int pid = fork();
  // printf(1, "Child 1 created\n");

  if (pid < 0)
  {
    printf(1, "first fork failed\n");
    exit();
  }

  if (pid == 0)
  {
    int secondPid = fork();
    // printf(1, "Child 2 created\n");

    if (secondPid < 0)
    {
      printf(1, "second fork failed\n");
      exit();
    }

    if (secondPid == 0)
    {
      int thirdPid = fork();
      // printf(1, "Child 3 created\n");

      if (thirdPid < 0)
      {
        printf(1, "third fork failed\n");
        exit();
      }

      if (thirdPid == 0)
      {
        printf(1, "Child 3 running\n");
        myprog(argc, argv);

        exit();
      }
      else
      {
        wait();
        printf(1, "Child 2 running\n");
        myprog(argc, argv);
      }
    }
    else
    {
      wait();
      printf(1, "Child 1 is running\n");
      myprog(argc, argv);
    }
  }
  else
  {
    // parent
    wait();
    printf(1, "Parent running\n");
    myprog(argc, argv);
  }
  exit();
}

void myprog(int argc, char *argv[])
{
  // print args
  int a, b;
  if (argc == 3)
  {
    a = atoi(argv[1]);
    b = atoi(argv[2]);
  }
  else
  {
    printf(1, "Usage: myprog a b\n");
    exit();
  }
  int c = mycall(a, b);
  printf(1, "my_scall(%d, %d) = %d\n", a, b, c);
  exit();
}
