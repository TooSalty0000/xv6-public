#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    oprintf(1, "Usage: genustest num_forks\n");
    exit();
  }

  int num_forks = atoi(argv[1]);

  if (num_forks < 1)
  {
    oprintf(1, "Invalid number of forks\n");
    exit();
  }

  if (num_forks > 63)
  {
    oprintf(1, "Number of forks exceeds maximum (63)\n");
    exit();
  }

  oprintf(1, "------------Start Genus Stress Test------------\n");
  for (int i = 0; i < num_forks; i++)
  {
    // Fork a child process
    int pid = fork();

    // Child process
    if (pid == 0)
    {
      // Get child process ID
      int child_pid = getpid();

      // Set genus
      int gid = setgenus(1);
      if (gid == 0)
      {
        oprintf(1, "Process %d failed to set genus\n", child_pid);
      }
      else
      {
        oprintf(1, "Process %d with genus ID %d\n", child_pid, gid);
      }

      // Get genus and capacity
      int gid2 = getgenus();
      int capacity = getcapacity();
      oprintf(1, "Process %d: test genus ID %d and c value %d\n", child_pid, gid2, capacity);

      // Sleep for 1 second
      oprintf(1, "Process %d: Sleeping for 1 seconds\n", child_pid);
      sleep(1000);

      // Exit child process
      oprintf(1, "Process %d: Exiting\n", child_pid);
      exit();
    }
  }

  // Wait for child processes to finish
  for (int i = 0; i < num_forks; i++)
  {
    wait();
  }

  oprintf(1, "------------End Genus Stress Test------------\n");
  exit();
}