#include "syscall-nr.h"

#define WRAPPER(name) \
  .globl _ ## name; \
_ ## name: \
    movl $SYSCALL_ ## name, %eax; \
    int $0x82; \
    ret

WRAPPER(task_exit)
WRAPPER(task_create)
WRAPPER(task_getid)
WRAPPER(task_yield)
WRAPPER(task_wait)
WRAPPER(reboot)
WRAPPER(mmap)
WRAPPER(munmap)
WRAPPER(sleep)
WRAPPER(nanosleep)
WRAPPER(beep)
WRAPPER(vm86)
WRAPPER(putchar)
WRAPPER(getchar)
WRAPPER(recv)
WRAPPER(send)
WRAPPER(ioctl)

WRAPPER(time)  /*第一次实验添加项*/

WRAPPER(getpriority)
WRAPPER(setpriority)/*第三次实验添加项*/

WRAPPER(sem_create)
WRAPPER(sem_destroy)
WRAPPER(sem_wait)
WRAPPER(sem_signal)/*第四次实验添加项*/

