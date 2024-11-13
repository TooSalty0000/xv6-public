#include "oputc.h"
#include "types.h"
#include "defs.h"
#include "spinlock.h"

// Define constants
#define MAX_OUTPUT_BUFFERS 1024
#define OUTPUT_BUFFER_SIZE 256

typedef struct OutputBuffer
{
  int id; // Incremental ID
  int fd;
  char buffer[OUTPUT_BUFFER_SIZE];
  int size;
  struct OutputBuffer *next;
} OutputBuffer;

static struct
{
  struct spinlock lock;
  OutputBuffer buffers[MAX_OUTPUT_BUFFERS];
  OutputBuffer *buffer_head; // Head of the OutputBuffer linked list
  int curpfid;
  int endpfid;
} oprintf_state;

// Initialize the oprintf state
void oprintf_init(void)
{
  initlock(&oprintf_state.lock, "oprintf");
  oprintf_state.buffer_head = 0;
  oprintf_state.curpfid = 0;
  oprintf_state.endpfid = 0;
  // Initialize the buffer pool
  for (int i = 0; i < MAX_OUTPUT_BUFFERS; i++)
  {
    oprintf_state.buffers[i].size = 0;
    oprintf_state.buffers[i].next = 0;
  }
}

// Function to handle character output or queuing
void oputc(int fd, char c, int id)
{
  acquire(&oprintf_state.lock);

  int curId = getpfid(); // Get the current ID being printed

  if (id == curId)
  {
    if (c == 0)
    {
      setpfid(curId + 1); // Update the current ID
      release(&oprintf_state.lock);
      return;
    }
    // If it's the current ID, print immediately
    consputc(c); // Use consputc for kernel-level character output
  }
  else
  {
    // Find or create an OutputBuffer for this ID
    OutputBuffer *buf = oprintf_state.buffer_head;
    OutputBuffer *prev_buf = 0;
    while (buf != 0 && buf->id != id)
    {
      prev_buf = buf;
      buf = buf->next;
    }
    if (buf == 0)
    {
      // Find an unused buffer from the pool
      buf = 0;
      for (int i = 0; i < MAX_OUTPUT_BUFFERS; i++)
      {
        if (oprintf_state.buffers[i].size == 0)
        {
          buf = &oprintf_state.buffers[i];
          break;
        }
      }
      if (buf == 0)
      {
        // No available buffers
        release(&oprintf_state.lock); // Release lock before cprintf
        cprintf("oputc: no available buffers\n");
        return;
      }
      // Initialize the new buffer
      buf->id = id;
      buf->fd = fd;
      buf->size = 0;
      buf->next = 0;
      // Add to the linked list
      if (prev_buf == 0)
      {
        oprintf_state.buffer_head = buf;
      }
      else
      {
        prev_buf->next = buf;
      }
    }
    // Append character to the buffer
    if (buf->size < OUTPUT_BUFFER_SIZE)
    {
      buf->buffer[buf->size++] = c;
    }
    else
    {
      release(&oprintf_state.lock); // Release lock before cprintf
      cprintf("oputc: buffer overflow for id %d\n", id);
      return;
    }
  }

  if (c == 0)
  {
    oputc_end(id);
  }

  release(&oprintf_state.lock);
}

// Function called at the end of an oprintf call
void oputc_end(int id)
{
  // Assume lock is already held by oputc
  int curId = getpfid(); // Get the current ID being printed

  // Process any buffered outputs with the new curId
  while (1)
  {
    OutputBuffer *buf = oprintf_state.buffer_head;
    OutputBuffer *prev_buf = 0;
    int found = 0;
    while (buf != 0)
    {
      if (buf->id == curId)
      {
        // Print all characters in the buffer
        for (int i = 0; i < buf->size; i++)
        {
          char c = buf->buffer[i];
          if (c == 0)
          {
            setpfid(++curId); // Update the current ID
          }
          else
          {
            consputc(c); // Use consputc to output character
          }
        }
        // Remove the buffer from the list
        if (prev_buf == 0)
        {
          oprintf_state.buffer_head = buf->next;
        }
        else
        {
          prev_buf->next = buf->next;
        }
        // Reset the buffer for reuse
        buf->size = 0;
        buf->next = 0;
        found = 1;
        break; // Restart scanning since the list has changed
      }
      else
      {
        prev_buf = buf;
        buf = buf->next;
      }
    }
    if (!found)
    {
      break; // No buffer found for current curId
    }
  }
}

int getpfid(void)
{
  return oprintf_state.curpfid;
}

void setpfid(int pfid)
{
  oprintf_state.curpfid = pfid;
}

int queuepfid(void)
{
  acquire(&oprintf_state.lock);
  int id = oprintf_state.endpfid++;
  release(&oprintf_state.lock);
  return id;
}