#ifndef __KPRINTF_H
#define __KPRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

// Print a formatted string with the given wirte function.
// note: writefn cannot fail (negative return value will be ignored)
// returns the number of bytes written.
int vfkprintf(int (*writefn)(const char *buf, int len), const char *fmt, va_list arg);
int kprintf(int (*writefn)(const char *buf, int len), const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
