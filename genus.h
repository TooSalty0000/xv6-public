#include "param.h"
#include "spinlock.h"

struct genus
{
  unsigned int isSet; // Unique genus ID
  int cValue;         // Specific value 'c' for the genus
  int procCount;      // Number of processes in the genus
};

struct genus_table // Table to store all genus
{
  struct spinlock lock;
  struct genus genus[MAXGENUS];
  int total_c_value;
};
