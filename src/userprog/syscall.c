#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  
}

void halt(void) {
  filesys_done();
  power_off();
}

bool create(const char *file, unsigned initial_size) {
  return filesys_create(file, initial_size);
}

int open(const char *file) {
  int fd = -1;
  for (int i = 2; i < 130; i++) {
    if (opened_files[i] == NULL) {
      struct file* opened_file = filesys_open(file);
      if (opened_file != NULL) {
        fd = i;
        opened_files[fd] = opened_file;
      }
      break;
    }
  }
  return fd;
}

void close(int fd) {
  if (fd > 1 && fd < 130) {
    if (opened_files[fd] != NULL) {
      file_close(opened_files[fd]);
      opened_files[fd] = NULL;
    }
  }
}

int read(int fd, const void *buffer, unsigned size) {
  if (fd == 0) {
    for (unsigned i = 0; i < size ;i++) {
      *(uint8_t*) buffer = input_getc();
      buffer++;
    }
    
    
    return size; // förmodligen inte rätt
  }
  else {
    if (fd > 1 && fd < 130) {
      if (opened_files[fd] != NULL) {
        return file_read(opened_files[fd], buffer, size); //kan bli lite konverteringsfel från off_t
      }
    }
  }
  return -1;
}

int write(int fd, const void *buffer, unsigned size) {
  if (fd == 1) {
    putbuf(buffer, size);
    return size;
  }

  else {
    if (fd > 1 && fd < 130) {
      if (opened_files[fd] != NULL) {
        return file_write(opened_files[fd], buffer, size);
      }
    }
  }
  return -1;
}

void exit(int status) {
  thread_exit();
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int syscall_nr = *(int*) f->esp;

  switch(syscall_nr) {
    case SYS_HALT:
    {
      halt();
      break;
    }

    case SYS_CREATE:
    {
      const char *file = *(char**) (f->esp+4); //file?? ska vi char istället för void? kanske assert?
      unsigned size = *(unsigned*) (f->esp+8);
      f->eax = create(file, size);
      break;
    }

    case SYS_OPEN:
    {
      const void *file = *(void**) (f->esp+4);
      f->eax = open(file);
      break;
    }

    case SYS_CLOSE:
    {
      int fd = *(int*) (f->esp+4);
      close(fd);
      break;
    }

    case SYS_READ:
    {
      int fd = *(int*) (f->esp+4);
      const void *buf = *(void**) (f->esp+8);
      unsigned size = *(unsigned*) (f->esp+12);
      f->eax = read(fd, buf, size);
      break;
    }
    
    case SYS_WRITE:
    {
      int fd = *(int*) (f->esp+4);
      const void *buf = *(void**) (f->esp+8);
      unsigned size = *(unsigned*) (f->esp+12);
      
      f->eax = write(fd, buf, size);
      break;
    }

    case SYS_EXIT: 
    {
      exit(0);
      break;
    }
    default:
    {
      printf("unkown syscall...");
      break;
    }
  }
}


