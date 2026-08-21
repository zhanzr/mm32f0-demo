/***********************************************************************************************************************
  @file    syscalls.c
  @brief   Minimal newlib syscall stubs for bare-metal (used together with -nostartfiles).
***********************************************************************************************************************/

#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>

extern char _sheap;
extern char _eheap;

void *_sbrk(ptrdiff_t incr)
{
    static char *heap_end = 0;
    char        *prev_heap_end;

    if (heap_end == 0)
    {
        heap_end = &_sheap;
    }

    prev_heap_end = heap_end;

    if (heap_end + incr > &_eheap)
    {
        errno = ENOMEM;
        return (void *)-1;
    }

    heap_end += incr;

    return (void *)prev_heap_end;
}

int _read(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;
    errno = ENOSYS;
    return -1;
}

int _close(int file)
{
    (void)file;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}

void _exit(int status)
{
    (void)status;
    for (;;)
    {
    }
}

int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    return -1;
}

int _getpid(void)
{
    return 1;
}