#include "types.h"
#include "stat.h"
#include "user.h"
#include "random.h"

#define ARRAY_SIZE 2

int main(int argc, char *argv[])
{
  int sample_size = 1000;

  if (argc >= 2)
  {
    sample_size = atoi(argv[1]);
  }

  int array[ARRAY_SIZE] = {10, 30};
  int counts[ARRAY_SIZE] = {0};
  int total = 0;

  // Verify the array sums to 100
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    total += array[i];
  }

  // if (total != 100)
  // {
  //   oprintf(1, "Array does not sum to 100.\n");
  //   return 1;
  // }

  // Seed the random number generator
  srand(uptime() * 13539027 + 123456789);

  // Sample 1000 times with lottery scheduling
  for (int i = 0; i < sample_size; i++)
  {
    int random_number = rand_int() % total; // Random number between 0 and 100
    int sum = 0;
    for (int j = 0; j < ARRAY_SIZE; j++)
    {
      sum += array[j];
      if (sum > random_number)
      {
        counts[j]++;
        break;
      }
    }
  }

  // Print the results
  oprintf(1, "Sampling distribution:\n");
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    oprintf(1, "Index %d: %d times\n", i, counts[i]);
  }

  exit();
}