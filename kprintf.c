#include <stdlib.h>
#include <stdarg.h>

typedef int (*kprintf_write_fn_t) (const char *buf, int len);

kprintf_write_fn_t kprintf_stdout = NULL;

// working buffer size, allocated on stack, this is not the maximal string size
#define KPRINTF_STACK_BUFFER_SIZE 30


#define HEX_PRINT_MIN_SZ 10 // TODO


// write maximum size bytes, advance fmt pointer
// fmt is set to NULL when done, no '\0' terminator is written
// returns the number of bytes written
static int print_to_buffer(char *buf, size_t size, const char **fmt, va_list arg)
{
    int remaining_size = size;
    while () {
        if (**fmt == '\0') {
            *fmt = NULL; // all done
            break;
        }
        if (**fmt == '%') {
            // d, i
            // u
            // x, X
            // f, F, e, E, g, G, a, A -> double
            // p
            if (*(*fmt + 1) == 'x') {
                if (remaining_size >= HEX_PRINT_MIN_SZ) {
                    (*fmt) += 2;
                    int 
                }
                continue;
            }
        }
        if (remaining_size >= 1) {
            *buf++ = **fmt;
            (*fmt)++;
            remaining_size--;
        } else { // out of buffer size
            break;
        }
    }
    return size - remaining_size;
}

int snkprintf(char * s, size_t n, const char * fmt, ...)
{

}


int vfkprintf(kprintf_write_fn_t writefn, const char *fmt, va_list arg)
{
    char buf[KPRINTF_STACK_BUFFER_SIZE];
    int ret = 0;
    while (fmt != NULL) {
        int len = print_to_buffer(buf, sizeof(buf), &fmt, arg);
        if (fmt != NULL && len == 0) {
            return -1; // not done printing but couldn't print anything
        }
        int wlen = writefn(buf, len);
        if (wlen != len) {
            return -1; // not all bytes were written
        }
        ret += len;
    }
    return ret;
}

int kprintf(const char *fmt, ...)
{
    int ret = -1;
    va_list args;
    va_start(args, fmt);
    if (kprintf_stdout) {
        ret = vfkprintf(kprintf_stdout, fmt, args);
    }
    va_end(args);
    return ret;
}
