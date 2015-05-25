#ifndef KPRINTF_H
#define KPRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdlib.h>

// Print a formatted string with the given wirte function.
// note: writefn cannot fail (negative return value will be ignored)
// returns the number of bytes written.
int vfkprintf(int (*writefn)(void *arg, const char *str, int len),
    void *parg, const char *fmt, va_list arg);
int kprintf(int (*writefn)(void *arg, const char *str, int len),
    void *parg, const char *fmt, ...);

// returns the number of bytes that would have been written.
// the string will always be '\0'-terminated.
// note: if the return value >= size, then data was discarded.
int snkprintf(char *str, size_t size, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
