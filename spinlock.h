// Mutual exclusion lock.
#ifndef XV6_SPINLOCK_H
#define XV6_SPINLOCK_H
struct spinlock
{
  uint locked; // Is the lock held?

  // For debugging:
  char *name;      // Name of lock.
  struct cpu *cpu; // The cpu holding the lock.
  uint pcs[10];    // The call stack (an array of program counters)
                   // that locked the lock.
};
#endif
