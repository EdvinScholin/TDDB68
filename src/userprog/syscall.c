#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "userprog/process.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void validate_pointer(const void *p) {
  if (pagedir_get_page(thread_current()->pagedir, p) != NULL && is_user_vaddr(p)) {
    return;
  }

  exit(-1);
}

void validate_string(const char* string) {
  if (string == NULL) {
    exit(-1);
  }
  else {
    for (char* str = string; str!='/0'; str++) {
      validate_pointer(str);
    }
  }
}

void validate_buffer(void* buffer, unsigned size) {
  for (unsigned i = 0; i < size; i++) {
    validate_pointer(buffer+i);
  }
}

void halt(void) {
  //filesys_done();
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
    
    return size; 
  }
  else {
    if (fd > 1 && fd < 130) {
      if (opened_files[fd] != NULL) {
        return file_read(opened_files[fd], buffer, size);
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
  thread_current()->pc->exit_status = status;
  thread_exit();
}

pid_t exec(const char *cmd_line) {
  pid_t pid = process_execute(cmd_line);
  return pid;
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  validate_pointer(f->esp);
  int syscall_nr = *(int*) f->esp;

  switch(syscall_nr) {
    case SYS_HALT:
    {
      halt();
      break;
    }

    case SYS_CREATE:
    {
      validate_pointer(f->esp+4);
      validate_pointer(f->esp+8);
      const char *file = *(char**) (f->esp+4);
      validate_string(file);
      unsigned size = *(unsigned*) (f->esp+8);
      f->eax = create(file, size);
      break;
    }

    case SYS_OPEN:
    {
      validate_pointer(f->esp+4);
      const void *file = *(void**) (f->esp+4);
      validate_string(file);
      f->eax = open(file);
      break;
    }

    case SYS_CLOSE:
    {
      validate_pointer(f->esp+4);
      int fd = *(int*) (f->esp+4);
      close(fd);
      break;
    }

    case SYS_READ:
    {
      validate_pointer(f->esp+4);
      validate_pointer(f->esp+8);
      validate_pointer(f->esp+12);
      int fd = *(int*) (f->esp+4);
      const void *buf = *(void**) (f->esp+8);
      unsigned size = *(unsigned*) (f->esp+12);
      validate_buffer(buf, size);

      f->eax = read(fd, buf, size);
      break;
    }
    
    case SYS_WRITE:
    {
      validate_pointer(f->esp+4);
      validate_pointer(f->esp+8);
      validate_pointer(f->esp+12);
      int fd = *(int*) (f->esp+4);
      const void *buf = *(void**) (f->esp+8);
      unsigned size = *(unsigned*) (f->esp+12);
      validate_buffer(buf, size);
      
      f->eax = write(fd, buf, size);
      break;
    }

    case SYS_EXIT: 
    {
      exit(0);
      break;
    }

    case SYS_EXEC: 
    {
      validate_pointer(f->esp+4);
      const char *cmd_line = *(char**) (f->esp+4);
      validate_string(cmd_line);
      f->eax = exec(cmd_line);
      break;
    }
    default:
    {
      printf("unkown syscall...");
      break;
    }
  }
}


