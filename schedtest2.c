#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAX_TICKS 1000 // Define the maximum number of ticks

struct loop_data
{
  int gid;
  int loop;
};

int main(int argc, char *argv[])
{
  // Parameters for the test
  int capacity[] = {30, 10, 0};
  int child_count[] = {2, 3, 1};

  int len = sizeof(capacity) / sizeof(int);

  int total_child_count = 0;
  int cummulative_child_count[len];
  for (int i = 0; i < len; i++)
  {
    cummulative_child_count[i] = total_child_count;
    total_child_count += child_count[i];
  }

  // Record the start time using uptime()
  int start_ticks = uptime();

  // Create pipes for each worker process
  int loopfd[total_child_count][2];
  for (int i = 0; i < total_child_count; i++)
  {
    if (pipe(loopfd[i]) < 0)
    {
      oprintf(1, "Error: pipe failed\n");
      exit();
    }
  }

  for (int i = 0; i < len; i++)
  {
    int c = capacity[i];
    int n = child_count[i];

    int pid = fork();
    if (pid < 0)
    {
      oprintf(1, "Error: fork failed\n");
      exit();
    }
    if (pid == 0)
    {
      // Child process (Group leader)

      int gid;
      if (c != 0)
      {
        gid = setgenus(c);
      }
      else
      {
        gid = 0;
      }

      oprintf(1, "Process %d: Setting genus %d with c = %d\n", getpid(), gid, c);

      for (int j = 0; j < n; j++)
      {
        int cpid = fork();
        if (cpid < 0)
        {
          oprintf(1, "Error: fork failed\n");
          exit();
        }
        if (cpid == 0)
        {
          // Grandchild process (Actual worker)

          int child_pid = getpid();
          int capacity;
          if (c != 0)
          {
            gid = getgenus();
            capacity = getcapacity();
          }
          else
          {
            gid = 0;
            capacity = 0;
          }
          oprintf(1, "Process %d: genus %d with c = %d\n", child_pid, gid, capacity);

          int loop = 0;

          while (1)
          {
            int current_ticks = uptime();
            int elapsed_ticks = current_ticks - start_ticks;

            // Check if the elapsed time has reached MAX_TICKS
            if (elapsed_ticks >= MAX_TICKS)
            {
              oprintf(1, "Process %d (Genus %d): Reached MAX_TICKS (%d). Exiting.\n", child_pid, gid, MAX_TICKS);
              break;
            }

            // int g = getgenus();
            int useless = 0;
            for (int k = 0; k < 1000000; k++)
            {
              useless++;
            }

            loop++;

            // oprintf(1, "Process %d (Genus %d): Uptime %d ticks, Loop %d\n", child_pid, g, current_ticks, loop);
          }

          // Send loop data to the parent
          struct loop_data data = {gid, loop};
          int worker_index = cummulative_child_count[i] + j;
          write(loopfd[worker_index][1], &data, sizeof(data));

          // Close the write end of the pipe
          close(loopfd[worker_index][1]);

          // Exit the worker process
          exit();
        }
      }
      // Group leader waits for all its child processes (grandchildren)
      for (int j = 0; j < n; j++)
      {
        wait();
      }
      // Exit the group leader
      exit();
    }
  }

  // Parent process waits for all group leaders (child processes)
  for (int i = 0; i < len; i++)
  {
    wait();
  }

  // Now collect loop data from all worker processes
  int total_loop = 0;
  struct loop_data data_arr[total_child_count];
  int gid_list[10];
  int gid_loops[10];
  int num_genus = 0;

  // Initialize genus arrays
  for (int i = 0; i < 10; i++)
  {
    gid_list[i] = -1; // Invalid genus ID
    gid_loops[i] = 0;
  }

  // Close unused write ends in the parent process
  for (int i = 0; i < total_child_count; i++)
  {
    close(loopfd[i][1]);
  }

  // Read loop data from each worker process
  for (int i = 0; i < total_child_count; i++)
  {
    struct loop_data data;
    read(loopfd[i][0], &data, sizeof(data));

    // Close the read end after reading
    close(loopfd[i][0]);

    data_arr[i] = data;
    total_loop += data.loop;

    // Accumulate loops per genus
    int found = 0;
    for (int j = 0; j < num_genus; j++)
    {
      if (gid_list[j] == data.gid)
      {
        gid_loops[j] += data.loop;
        found = 1;
        break;
      }
    }
    if (!found)
    {
      gid_list[num_genus] = data.gid;
      gid_loops[num_genus] = data.loop;
      num_genus++;
    }
  }

  // Print the results
  oprintf(1, "Total Loop: %d\n", total_loop);

  oprintf(1, "Loop Counts per Process:\n");
  for (int i = 0; i < total_child_count; i++)
  {
    oprintf(1, "Process %d(%d): Loop %d\n", i, data_arr[i].gid, data_arr[i].loop);
  }

  oprintf(1, "Loop Counts per Genus:\n");
  for (int i = 0; i < num_genus; i++)
  {
    oprintf(1, "Genus %d: Loop %d\n", gid_list[i], gid_loops[i]);
  }

  oprintf(1, "Loop Percentages:\n");
  for (int i = 0; i < num_genus; i++)
  {
    int percentage = gid_loops[i] * 100 / total_loop;
    oprintf(1, "Genus %d: %d%%\n", gid_list[i], percentage);
  }

  oprintf(1, "Parent: All child processes have exited. Exiting.\n");
  exit();
}