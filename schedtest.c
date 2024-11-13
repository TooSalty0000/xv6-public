#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_TICKS 1000

struct loop_data
{
  int gid;
  int loop;
};

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    oprintf(1, "Usage: %s <c0> ... <cn>\n", argv[0]);
    oprintf(1, "where c0, ..., cn are the c values for each genus\n");
    oprintf(1, "max n = 10\n");
    exit();
  }

  int tick0 = uptime();

  int loopfd[argc][2];
  for (int i = 0; i < argc; i++)
  {
    if (pipe(loopfd[i]) < 0)
    {
      oprintf(1, "Error: pipe failed\n");
      exit();
    }
  }

  for (int i = 1; i < argc + 1; i++)
  {
    if (i > 10)
    {
      oprintf(1, "Error: max number of genus is 10\n");
      exit();
    }
    int c;
    if (i == argc)
    {
      c = 0;
    }
    else
    {
      c = atoi(argv[i]);
    }
    if (c < 1 && i != argc)
    {
      oprintf(1, "Error: c value must be a natural number\n");
      exit();
    }

    int pid = fork();
    if (pid < 0)
    {
      oprintf(1, "Error: fork failed\n");
      exit();
    }
    if (pid == 0)
    {
      int child_pid = getpid();
      int gid, capacity;
      if (c != 0)
      {
        gid = setgenus(c);
        capacity = getcapacity();
      }
      else
      {
        gid = 0;
        capacity = 0;
      }
      oprintf(1, "Process %d: Setting genus %d with c = %d\n", child_pid, gid, capacity);
      int loop = 0;
      while (1)
      {
        int cpid = getpid();
        int g = getgenus();
        int useless = 0;
        for (int j = 0; j < 1000000; j++)
        {
          useless += 1;
          asm("nop");
        }
        loop++;
        int tick = uptime();
        if (tick - tick0 > MAX_TICKS)
        {
          break;
        }
      }
      oprintf(1, "Process %d: Exiting with Loop %d\n", child_pid, loop);
      struct
      {
        int gid;
        int loop;
      } data = {gid, loop};
      write(loopfd[i - 1][1], &data, sizeof(data));
      close(loopfd[i - 1][1]);
      exit();
    }
  }

  // Parent process sleep for 100000 ticks
  // sleep(100000);
  for (int i = 0; i < argc; i++)
  {
    int cpid = wait();
  }
  int total_loop = 0;
  struct loop_data data_arr[argc];
  oprintf(1, "\n--------------------------------\n");
  oprintf(1, "Loop Counts per Process:\n");
  for (int i = 0; i < argc; i++)
  {
    struct loop_data data;
    read(loopfd[i][0], &data, sizeof(data));
    oprintf(1, "Genus %d: Loop %d\n", data.gid, data.loop);
    data_arr[i] = data;
    total_loop += data.loop;
  }
  oprintf(1, "\nTotal Loop: %d\n", total_loop);
  for (int i = 0; i < argc; i++)
  {
    close(loopfd[i][0]);
  }
  oprintf(1, "\nLoop Percentages:\n");
  for (int i = 0; i < argc; i++)
  {
    oprintf(1, "Genus %d: %d%%\n", data_arr[i].gid, data_arr[i].loop * 100 / total_loop);
  }

  exit();
}