#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
  if (argc < 2 || argc > 11)
  {
    oprintf(1, "Usage: genustest <c1> <c2> ... <cn>\n");
    oprintf(1, "Maximum of 10 c values\n");
    exit();
  }

  int c[10];
  int num_values = argc - 1;

  // Parse c values from command-line arguments
  for (int i = 0; i < num_values; i++)
  {
    c[i] = atoi(argv[i + 1]);
  }

  // Create child processes
  for (int i = 0; i < num_values; i++)
  {
    if (i == 0)
    {
      // parent process first run
      oprintf(1, "------------Start Genus Test------------\n");
      oprintf(1, "Parent process %d\n", getpid());
      oprintf(1, "Unique Child Generation\n");
      oprintf(1, "-----------------------------------------\n");
    }
    int pid = fork();
    if (pid < 0)
    {
      oprintf(1, "Fork failed\n");
      exit();
    }
    else if (pid == 0 && i == 0)
    {
      oprintf(1, "------------First Child Process------------\n");
      int gid = setgenus(c[i]);
      int childPid = getpid();
      if (gid == 0)
      {
        oprintf(1, "Process %d failed to set genus with c value %d\n", childPid, c[i]);
        exit();
      }
      else
      {
        int genus_id = getgenus();
        oprintf(1, "Process %d with genus ID %d and c value %d\n", childPid, genus_id, c[i]);

        // create two more child processes and try to set genus
        for (int j = 0; j < 2; j++)
        {
          int pid2 = fork();
          if (pid2 < 0)
          {
            oprintf(1, "Fork failed\n");
            exit();
          }
          else if (pid2 == 0)
          {
            oprintf(1, "---------Grandchild Process---------\n");
            int gid2 = setgenus(c[i]);
            int childPid2 = getpid();
            if (gid2 == 0)
            {
              oprintf(1, "Process %d failed to set genus with c value %d\n", childPid2, c[i]);
              oprintf(1, "Current genus ID: %d\n", getgenus());
              oprintf(1, "My c value: %d\n", getcapacity());
            }
            else
            {
              int genus_id2 = getgenus();
              oprintf(1, "Process %d with genus ID %d and c value %d\n", childPid2, genus_id2, c[i]);
            }
            exit();
          }
          wait();
          oprintf(1, "-----------------------------------\n");
        }
      }
      // End of grandchild process creation loop

      // Exit child process
      exit();
    }
    else if (pid == 0 && i == 1)
    {
      oprintf(1, "------------Second Child Process------------\n");
      int childPid = getpid();
      int genus_id = getgenus();
      int c_value = getcapacity();
      oprintf(1, "Process %d with genus ID %d and c value %d\n", childPid, genus_id, c_value);

      // create two more child processes and try to set genus
      for (int j = 0; j < 2; j++)
      {
        if (j == 0)
        {
          oprintf(1, "---------Grandchild Parallel Process---------\n");
        }
        int pid2 = fork();
        if (pid2 < 0)
        {
          oprintf(1, "Fork failed\n");
          exit();
        }
        else if (pid2 == 0)
        {
          int gid2 = setgenus(c[i]);
          int childPid2 = getpid();
          if (gid2 == 0)
          {
            oprintf(1, "Process %d failed to set genus with c value %d\n", childPid2, c[i]);
            exit();
          }
          else
          {
            int genus_id2 = getgenus();
            int c_value = getcapacity();
            oprintf(1, "Process %d with genus ID %d and c value %d\n", childPid2, genus_id2, c_value);
            oprintf(1, "Process %d: Sleeping for 1 second\n", childPid2);
            sleep(1000);
            oprintf(1, "Process %d: exiting\n", childPid2);
            exit();
          }
        }
      }
      // Wait for grandchild processes to finish
      for (int j = 0; j < 2; j++)
      {
        wait();
      }
      oprintf(1, "-----------------------------------\n");
      // End of grandchild process creation loop

      // Exit child process
      exit();
    }
    else if (pid == 0)
    {
      oprintf(1, "------------Child Process------------\n");
      // Child process
      int gid = setgenus(c[i]);
      int capacity = getcapacity();
      int childPid = getpid();
      if (gid == 0)
      {
        oprintf(1, "Process %d failed to set genus with c value %d\n", childPid, capacity);
        exit();
      }
      else
      {
        int genus_id = getgenus();
        int capacity = getcapacity();
        oprintf(1, "Process %d with genus ID %d and c value %d\n", childPid, genus_id, capacity);
      }
      // Exit child process
      exit();
    }
    // Parent process waits for child process to finish
    wait();
    // End of child process creation loop
    oprintf(1, "-----------------------------------------\n");
  }

  // Test parallel forking
  for (int i = 0; i < num_values; i++)
  {
    if (i == 0)
    {
      // parent process first run
      oprintf(1, "------------Parallel Forking------------\n");
      oprintf(1, "Parent process %d\n", getpid());
      oprintf(1, "-----------------------------------------\n");
    }
    oprintf(1, "Parent: Loop %d\n", i + 1);
    int pid = fork();
    if (pid < 0)
    {
      oprintf(1, "Fork failed\n");
      exit();
    }
    else if (pid == 0)
    {
      // Child process
      int gid = setgenus(c[i]);
      int childPid = getpid();
      if (gid == 0)
      {
        oprintf(1, "Child process %d: failed to set genus with c value %d\n", childPid, c[i]);
        exit();
      }
      else
      {
        int genus_id = getgenus();
        int capacity = getcapacity();
        oprintf(1, "Child process %d: created with genus ID %d and c value %d\n", childPid, genus_id, capacity);
      }
      oprintf(1, "Child process %d: Sleeping for 1 second\n", childPid);
      sleep(1000);
      oprintf(1, "Child process %d: exiting\n", childPid);
      // Exit child process
      exit();
    }
  }

  for (int i = 0; i < num_values; i++)
  {
    // Parent process waits for child process to finish
    wait();
  }

  exit();
}