// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  char lock_name[7];
} kmem[NCPU];

void
kinit()
{
  for(int i=0;i<NCPU;i++)
  {
    snprintf(kmem[i].lock_name, sizeof(kmem[i].lock_name), "kmem_%d", i);
    initlock(&kmem[i].lock, "kmem");
  }
  freerange(end, (void*)PHYSTOP); //在此处初始化了freelist
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  int id = cpuid();
  acquire(&kmem[id].lock);
  r->next = kmem[id].freelist;
  kmem[id].freelist = r;
  release(&kmem[id].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r,*p;

  int id = cpuid();

  acquire(&kmem[id].lock);
  r = kmem[id].freelist;
  if(r)
    kmem[id].freelist = r->next;
  else
  {
    //try to steal from other cpu
    int i=0;
    for(i=0;i<NCPU;i++)
    {
      if (i == id)
        continue;
      p=kmem[i].freelist;
      if(p)
      {
        acquire(&kmem[i].lock);
        kmem[i].freelist=p->next;
        r=p; //steal
        release(&kmem[i].lock);
        break;
      }
    }
  }
  release(&kmem[id].lock);
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
