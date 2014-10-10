#ifndef __KPRINTF_H
#define __KPRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

typedef int (*kprintf_write_fn_t) (const char *buf, int len);

extern kprintf_write_fn_t kprintf_stdout;

int vfkprintf(kprintf_write_fn_t writefn, const char *fmt, va_list arg);

int kprintf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
