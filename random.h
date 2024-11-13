#ifndef RANDOM_H
#define RANDOM_H

static unsigned int seed = 123456789; // Initial seed value, can be changed

// Function to set the seed for reproducibility
void srand(unsigned int s)
{
  seed = s;
}

// Function to generate a pseudo-random number between 0 and 1
int rand_int()
{
  // Linear Congruential Generator (LCG) formula
  seed = (1103515245 * seed + 12345) & 0x7fffffff;
  return seed;
}

float rand_float()
{
  // Linear Congruential Generator (LCG) formula
  seed = (1103515245 * seed + 12345) & 0x7fffffff;
  return (float)seed / (float)0x7fffffff;
}

#endif