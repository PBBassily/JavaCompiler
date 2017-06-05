#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "user/syscall.h"
#include <stdbool.h>
#include <debug.h>
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include "threads/malloc.h"
#include "devices/input.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
//*******************************************************************************************
static int add_file(struct file *f);
static struct fd_mapper* get_file(int fd);
static void syscall_handler(struct intr_frame *);
static void extract_args(void* p, int argv[], int argc);
static bool validate_address(const void* p);
//********************************************************************************************

void syscall_init(void) {
  lock_init(&fs_lock);
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

//*****************************************************************************
/*Terminates the current user program, returning status to the kernel. If the
  process's parent waits for it , this is the status that will be returned.
  Conventionally, a status of 0 indicates success and nonzero values indicate errors.
*/
void exit(int status) {

  struct thread* current_thread = thread_current();

  printf ("%s: exit(%d)\n", current_thread->name, status);

  struct list_elem* e;

  if(thread_current()->parent!=NULL){

    for (e = list_begin (&thread_current()->parent->children);
      e != list_end (&thread_current()->parent->children); e = list_next (e))
    {
      struct child *c = list_entry (e, struct child, elem);

      if(c->child_pid == thread_current()->tid){
        c->status = status;
        switch(status){
          case -1:
            c->exs = KILLED;
            break;
          default:
            c->exs = NORMAL_EXIT;
            break;
          }
          break;
      }
    }
  }

  thread_exit();
}
//*****************************************************************************
/*Runs the executable whose name is given in cmd_line, passing any given arguments
  , and returns the new process's program id (pid). Must return pid -1, which
  otherwise should not be a valid pid, if the program cannot load or run for
  any reason. Thus, the parent process cannot return from the exec until it knows
  whether the child process successfully loaded its executable.
*/
pid_t execute(const char *cmd_line) {

  struct thread* parent_thread = thread_current();
  //create child process with certain pid for the current thread and set the pid_ofChild with that pid*/
  pid_t pid = (pid_t) process_execute(cmd_line); /*create thread with given cmd_line*/

  if(pid==PID_ERROR){
    return PID_ERROR;
  }

  struct list_elem* e;

  for (e = list_begin (&parent_thread->children);
    e != list_end (&parent_thread->children); e = list_next (e))
  {
    struct child *c = list_entry (e, struct child, elem);

    if(c->child_pid==pid){
      //the parent process should wait for its childern to be executed so it can return from exec
      sema_down(&c->child_t->execSema);
      if(!c->loaded){
        return -1;
      }
      break;
    }
  }

  return pid;
}
//*****************************************************************************
/*
  Waits for a child process pid and retrieves the child's exit status.
  If pid is still alive, waits until it terminates.
  Then, returns the status that pid passed to exit.
  If pid did not call exit(), but was terminated by the kernel,
  wait(pid) must return -1. It is perfectly legal for a parent process to wait for child processes
  that have already terminated by the time the parent calls wait,
  but the kernel must still allow the parent to retrieve its child's exit status
  or learn that the child was terminated by the kernel.
*/
int wait(pid_t pid) {
  return process_wait(pid);
}
//*****************************************************************************
/*Terminates Pintos by calling shutdown_power_off()
  (declared in "threads/init.h").
  This should be seldom used, because you lose some information about possible
  deadlock situations, etc.
*/
void halt(void) {
  shutdown_power_off();
}
//*****************************************************************************
/*Creates a new file called file initially initial_size bytes in size. Returns
  true if successful, false otherwise. Creating a new file does not open it:
  opening the new file is a separate operation which would require a open
  system call.
  */
bool create(const char *file, unsigned initial_size) {
  lock_acquire(&fs_lock);

  bool state = filesys_create(file, initial_size);

  lock_release(&fs_lock);

  return state;
}
//*****************************************************************************
/*Deletes the file called file. Returns true if successful, false otherwise.
  A file may be removed regardless of whether it is open or closed, and
  removing an open file does not close it.
*/
bool remove(const char *file) {
  lock_acquire(&fs_lock);

  bool state = filesys_remove(file);

  lock_release(&fs_lock);

  return state;
}
//*****************************************************************************
/*
  Opens the file called file. Returns a nonnegative integer handle called a
  "file descriptor" (fd), or -1 if the file could not be opened.
  File descriptors numbered 0 and 1 are reserved for the console:
  fd 0 (STDIN_FILENO) is standard input, fd 1 (STDOUT_FILENO) is standard output.
  The open system call will never return either of these file descriptors,
  which are valid as system call arguments only as explicitly described below.
*/
int open(const char *file) {
  lock_acquire(&fs_lock);

  struct file* f_opened = filesys_open(file);

  int fd = add_file(f_opened);

  lock_release(&fs_lock);
  return fd;
}
//*****************************************************************************
/*returns the file size in bytes and it has to be opened*/
int filesize(int fd) {
  lock_acquire(&fs_lock);
  struct fd_mapper* fd_map = get_file(fd);
  struct file* f_opened = fd_map->file;

  int size = file_length(f_opened);
  lock_release(&fs_lock);
  return size;
}
//*****************************************************************************
/*Reads size bytes from the file open as fd into buffer. Returns the number of
  bytes actually read (0 at end of file), or -1 if the file could not be
  read (due to a condition other than end of file).
  Fd 0 reads from the keyboard using input_getc()
*/
int read(int fd, void *buffer, unsigned length) {
  lock_acquire(&fs_lock);

  if (fd == 0) {
    unsigned i=0;
    while (i < length) {
      *((uint8_t *) buffer) = input_getc();
      buffer = ((uint8_t*) buffer) + 1;
      i++;
    }
    lock_release(&fs_lock);
    return length;
  }

  struct fd_mapper* fd_map = get_file(fd);

  if (fd_map == NULL) {
    lock_release(&fs_lock);
    exit(-1);
  }

  struct file* f_opened = fd_map->file;


  int actually_read = file_read(f_opened, buffer, length);

  lock_release(&fs_lock);

  return actually_read;
}
//*****************************************************************************
/*
  Writes size bytes from buffer to the open file fd. Returns the number of bytes
  actually written, which may be less than size if some bytes could not be written.
  Fd 1 writes to the console. Your code to write to the console should write all
  of buffer in one call to putbuf(),
*/
int write(int fd, const void *buffer, unsigned length) {
  lock_acquire(&fs_lock);

  if (fd == 1) {
    putbuf(buffer, length);
    lock_release(&fs_lock);
    return length;
  }


  struct fd_mapper* fd_map = get_file(fd);

  if(fd_map == NULL){
    lock_release(&fs_lock);
    exit(-1);
  }

  struct file* f_opened = fd_map->file;

  int actually_written = file_write(f_opened, buffer, length);

  lock_release(&fs_lock);

  return actually_written;
}
//*****************************************************************************
/*
  Changes the next byte to be read or written in open file fd to position,
  expressed in bytes from the beginning of the file.
*/
void seek(int fd, unsigned position) {
  lock_acquire(&fs_lock);

  struct fd_mapper* fd_map = get_file(fd);

  struct file* f_opened = fd_map->file;


  file_seek(f_opened, position);

  lock_release(&fs_lock);
}
//*****************************************************************************
/*
  Returns the position of the next byte to be read or written in open file fd,
  expressed in bytes from the beginning of the file.
*/
unsigned tell(int fd) {
  lock_acquire(&fs_lock);

  struct fd_mapper* fd_map = get_file(fd);

  struct file* f_opened = fd_map->file;


  off_t position = file_tell(f_opened);

  lock_release(&fs_lock);

  return position;
}
//********************************************************************************************
/*
  Closes file descriptor fd. Exiting or terminating a process implicitly closes all its open
  file descriptors, as if by calling this function for each one.
*/
void close(int fd) {
  lock_acquire(&fs_lock);

  struct fd_mapper* fd_map = get_file(fd);

  if(fd_map == NULL){
    lock_release(&fs_lock);
    exit(-1);
  }

  struct file* f_to_close = fd_map->file;

  file_close(f_to_close);

  hash_delete(&thread_current()->mapper, &fd_map->hash_elem);

  free(fd_map);

  lock_release(&fs_lock);
}
//********************************************************************************************
/* Adds the opened file "f" to the current thread's mapper
  giving it fd as file discriptor, return fd
*/
int add_file(struct file *f) {
  if (f == NULL)
    return -1;

  struct thread* t = thread_current();

  struct fd_mapper* newMapper = (struct fd_mapper*) malloc(sizeof(struct fd_mapper));
  newMapper->file = f;
  newMapper->fd = t->num_fd_mappers;
  t->num_fd_mappers++;
/*insert the FD into the hash of that current thread */
  hash_insert(&t->mapper, &(newMapper->hash_elem));
  int f_d = newMapper->fd;
  return f_d;
}
//********************************************************************************************
/* gets the file with "fd" from the current thread's mapper */
struct fd_mapper* get_file(int fd) {

  struct thread* t = thread_current();

  struct fd_mapper dummyMap;

  dummyMap.fd = fd;

  struct hash_elem* h_e = hash_find(&t->mapper, &(dummyMap.hash_elem));

  if (h_e == NULL)
    return NULL;

  struct fd_mapper* foundMap = hash_entry(h_e, struct fd_mapper, hash_elem);

  return foundMap;
}
//********************************************************************************************
/*
  get the args from the stack and return arg
*/
static void extract_args(void* p, int argv[], int argc){

  int i = 0;

  while(i < argc){

    argv[i] = *((int*)p);

    p += 4;

    i++;
  }

}
//********************************************************************************************
/*checks that given address to on any type is within range*/
static bool validate_address(const void* p){
  if( p>=PHYS_BASE)
    return false;

  void* kernalAddr = pagedir_get_page(thread_current()->pagedir, p);

  if(kernalAddr==NULL)
    return false;

  return true;
}
//********************************************************************************************
//********************************************************************************************
static void syscall_handler(struct intr_frame *f ) {

  const void* p = f->esp; /* esp points to the last element on stack */

  // check if the address in user space or not
  if(!validate_address( p)){
    exit(-1);
  }
  // the stack contains at first the number of system call
  int sys_call_num = *((int*) p);

  // increment the pointer
  p +=4;

  if(!validate_address(p)){
    exit(-1);
  }

  //max number of argumnets in all syscall methods is = 3
  int argv[3];

  // switch on system calls
  switch (sys_call_num) {

    case SYS_HALT: {
      halt();
    }break;

    case SYS_EXIT: {
      extract_args(p, argv, 1);
      exit(argv[0]);
    }break;

    case SYS_EXEC: {

      extract_args(p, argv, 1);

      if(!validate_address((const void*) argv[0]))
        exit(-1);

      f->eax = execute((const char*) argv[0]);
    }break;

    case SYS_WAIT: {
      extract_args(p, argv, 1);
      f->eax = wait(argv[0]);
    }break;

    case SYS_CREATE: {

      extract_args(p, argv, 2);

      if(!validate_address((const void*) argv[0]))
        exit(-1);

      f->eax = create((const char*) argv[0], (unsigned)argv[1]);
    }break;

    case SYS_REMOVE: {

      extract_args(p, argv, 1);

      if(!validate_address((const void*) argv[0]))
        exit(-1);
      f->eax = remove((const char*) argv[0]);
    }break;

    case SYS_OPEN: {

      extract_args(p, argv, 1);

      if(!validate_address((const void*) argv[0]))
        exit(-1);

      f->eax = open((const char*)argv[0]);
    }break;

    case SYS_FILESIZE: {
      extract_args(p, argv, 1);
      f->eax = filesize(argv[0]);
    }break;

    case SYS_READ: {

      extract_args(p, argv, 3);

      if(!validate_address((const void*) argv[1]))
        exit(-1);

      /*check buffer*/
      void* tempBuf = ((void*) argv[1])+ argv[2];

      if(!validate_address((const void*) tempBuf))
        exit(-1);

      f->eax = read(argv[0], (void*)argv[1], (unsigned)argv[2]);
    }break;

    case SYS_WRITE: {

      extract_args(p, argv, 3);

      if(!validate_address((const void*) argv[1]))
        exit(-1);

      /*check buffer*/
      void* tempBuf = ((void*) argv[1])+ argv[2];
      if(!validate_address((const void*) tempBuf))
        exit(-1);

      f->eax = write(argv[0], (void*)argv[1], (unsigned)argv[2]);
    }break;

    case SYS_SEEK: {
      extract_args(p, argv, 2);
      seek(argv[0], (unsigned)argv[1]);
    }break;

    case SYS_TELL: {
      extract_args(p, argv, 1);
      f->eax = tell(argv[0]);
    }break;

    case SYS_CLOSE: {
      extract_args(p, argv, 1);
      close(argv[0]);
    }break;

    default: {
      thread_exit();
    }break;
  }
}
//*********************************************************************************
