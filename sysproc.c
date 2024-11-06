#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "genus.h"
#include "my_scall.h"

int sys_fork(void)
{
  return fork();
}

int sys_exit(void)
{
  exit();
  return 0; // not reached
}

int sys_wait(void)
{
  return wait();
}

int sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getpid(void)
{
  return myproc()->pid;
}

int sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

int sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_my_scall(void)
{
  int a, b;
  argint(0, &a);
  argint(1, &b);
  int c = my_scall(a, b);
  return c;
}

int sys_setgenus(void)
{
  int c;
  argint(0, &c);
  int genus = setgenus_sys(c);
  return genus;
}

int sys_getgenus(void)
{
  return getgenus_sys();
}

int sys_getcapacity(void)
{
  return getcapacity_sys();
}

int sys_oputc(void)
{
  int fd;
  char c;
  int id;
  if (argint(0, &fd) < 0 || argint(1, &c) < 0 || argint(2, &id) < 0)
    return -1;
  oputc(fd, c, id);
  return 0;
}

int sys_qprint(void)
{
  return queuepfid();
}
