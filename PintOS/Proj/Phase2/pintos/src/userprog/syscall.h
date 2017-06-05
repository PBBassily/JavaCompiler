#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <debug.h>
#include "lib/user/syscall.h"

void syscall_init(void);
// lock to protect the system calls
struct lock fs_lock;

// SYS_HALT
void halt(void) NO_RETURN;
// SYS_EXIT
void exit(int status) NO_RETURN;
// SYS_EXEC
pid_t execute(const char *cmd_line);
// SYS_WAIT
int wait(pid_t pid);
// SYS_CREATE
bool create(const char *file, unsigned initial_size);
// SYS_REMOVE
bool remove(const char *file);
// SYS_OPEN
int open(const char *file);
// SYS_FILESIZE
int filesize(int fd);
// SYS_READ
int read(int fd, void *buffer, unsigned length);
// SYS_WRITE
int write(int fd, const void *buffer, unsigned length);
// SYS_SEEK
void seek(int fd, unsigned position);
// SYS_TELL
unsigned tell(int fd);
// SYS_CLOSE
void close(int fd);

#endif /* userprog/syscall.h */
