#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "genus.h"
#include "spinlock.h"

#define MAX_GENUS 64
#define MAX_C_VALUE 90

struct
{
  struct spinlock lock;
  struct genus genus[MAX_GENUS];
  int total_c_value;
} gtable;

// Initialize the genus table
void ginit(void)
{
  for (int i = 0; i < MAX_GENUS; i++)
  {
    gtable.genus[i].isSet = 0;
    gtable.genus[i].cValue = 0;
    gtable.genus[i].procCount = 0;
  }
  gtable.total_c_value = 0;
  initlock(&gtable.lock, "genus_lock");
}

int setgenus_sys(int c)
{
  // check c value if it is a natural number
  if (c < 1)
  {
    return 0;
  }

  struct proc *p = myproc();

  // If the genus is already set, return 0
  if (p->genus_id != 0)
  {
    return 0;
  }

  // if the total c value is greater than MAX_C_VALUE, return 0
  if (gtable.total_c_value + c > MAX_C_VALUE)
  {
    return 0;
  }

  // Find unused genus ID
  int id = 1;
  struct genus *g;
  acquire(&gtable.lock);
  for (g = gtable.genus; g < &gtable.genus[MAX_GENUS]; g++)
  {
    // If the genus is not set, set it
    if (g->isSet == 0)
    {
      g->isSet = 1;
      g->cValue = c;
      g->procCount = 1;
      gtable.total_c_value += c;
      p->genus_id = id;
      release(&gtable.lock);
      return id;
    }
    id++;
  }

  // If no genus ID is available, return 0
  release(&gtable.lock);
  return 0;
}

int getgenus_sys()
{
  struct proc *p = myproc();
  return p->genus_id;
}

int getcapacity_sys()
{
  struct proc *p = myproc();
  struct genus *g;
  acquire(&gtable.lock);
  g = &gtable.genus[p->genus_id - 1];
  release(&gtable.lock);
  if (g->isSet == 0)
  {
    return 0;
  }
  return g->cValue;
}

void addgenus(int gid)
{
  if (gid == 0)
  {
    return;
  }

  struct genus *g;
  acquire(&gtable.lock);
  g = &gtable.genus[gid - 1];
  if (g->isSet == 1)
  {
    g->procCount++;
  }
  else
  {
    panic("Genus not set");
  }
  release(&gtable.lock);
  return;
}

void removegenus(int gid)
{
  if (gid == 0)
  {
    return;
  }

  struct genus *g;
  acquire(&gtable.lock);
  g = &gtable.genus[gid - 1];
  if (g->isSet == 1 && g->procCount >= 1)
  {
    g->procCount--;
    if (g->procCount == 0)
    {
      gtable.total_c_value -= g->cValue;
      g->isSet = 0;
      g->cValue = 0;
    }
  }
  release(&gtable.lock);
  return;
}