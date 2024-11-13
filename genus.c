#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "genus.h"

struct genus_table gtable;

unsigned int cur_genus_id = 1;

// Initialize the genus table
void ginit(void)
{
  for (int i = 0; i < MAXGENUS; i++)
  {
    gtable.genus[i].isSet = 0;
    gtable.genus[i].cValue = 0;
    gtable.genus[i].procCount = 0;
  }
  gtable.total_c_value = 0;
  cur_genus_id = 1;
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
  if (gtable.total_c_value + c > MAXCVALUE)
  {
    return 0;
  }

  // Find unused genus ID
  struct genus *g;
  acquire(&gtable.lock);
  for (g = gtable.genus; g < &gtable.genus[MAXGENUS]; g++)
  {
    // If the genus is not set, set it
    if (g->isSet == 0)
    {
      int id = cur_genus_id++;
      if (cur_genus_id <= 0)
      {
        cur_genus_id = 1;
      }
      g->isSet = id;
      g->cValue = c;
      g->procCount = 1;
      gtable.total_c_value += c;
      p->genus_id = id;
      release(&gtable.lock);
      return id;
    }
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
  for (g = gtable.genus; g < &gtable.genus[MAXGENUS]; g++)
  {
    if (g->isSet != 0 && g->isSet == p->genus_id)
    {
      release(&gtable.lock);
      return g->cValue;
    }
  }
  release(&gtable.lock);
  if (g->isSet == 0)
  {
    return 0;
  }
  return g->cValue;
}

void addgenus(unsigned int gid)
{
  if (gid == 0)
  {
    return;
  }

  struct genus *g;
  acquire(&gtable.lock);
  for (g = gtable.genus; g < &gtable.genus[MAXGENUS]; g++)
  {
    if (g->isSet == gid)
    {
      g->procCount++;
      release(&gtable.lock);
      return;
    }
  }
  panic("addgenus: genus not found");
  release(&gtable.lock);
  return;
}

void removegenus(unsigned int gid)
{
  if (gid == 0)
  {
    return;
  }

  struct genus *g;
  acquire(&gtable.lock);
  for (g = gtable.genus; g < &gtable.genus[MAXGENUS]; g++)
  {
    if (g->isSet == gid)
    {
      g->procCount--;
      if (g->procCount == 0)
      {
        gtable.total_c_value -= g->cValue;
        g->isSet = 0;
        g->cValue = 0;
      }
      release(&gtable.lock);
      return;
    }
  }
  panic("removegenus: genus not found");
  release(&gtable.lock);
  return;
}