#include <stdlib.h>
#include <stdarg.h>

typedef int (*kprintf_write_fn_t) (const char *buf, int len);

kprintf_write_fn_t kprintf_stdout = NULL;

// working buffer size, allocated on stack, this is not the maximal string size
#define KPRINTF_STACK_BUFFER_SIZE 30


#define HEX_PRINT_MIN_SZ 10 // TODO


// %[flags][width][.precision][length]specifier

// flags:
// -        left-justify in padding
// +        force print sign
// space    print '-' if negative, ' ' otherwise
// #        print 0x or 0X (ignore a, A, e, E, f, F, g or G)
// 0        if padding is specified, fill with 0 instead of spaces

// width:
// number   print padding
// *        padding number is passed as preceding int argument

// .precision:
// .number  for int: minimum number of digits to be printed with leading zeroes
//          (if .number == 0, print nothing for value 0)
//          for float/double/etc: ignored
// .*       same as number, just passed as preceding int argument

// length:
// none     int
// hh       char
// h        short int
// l        long int
// ll       long long int
// j        intmax_t uintmax_t
// z        size_t
// t        ptrdiff_t
// L        long double

// specifiers:
// d, i
// u
// x, X
// c, s
// f, F, e, E, g, G, a, A -> double
// p
// %

// div: o (octal), n(return nb chars written via ptr)


// write maximum size bytes, advance fmt pointer
// fmt is set to NULL when done, no '\0' terminator is written
// returns the number of bytes written
static int print_to_buffer(char *buf, size_t size, const char **fmt, va_list arg)
{
    int remaining_size = size;
    while (1) {
        if (**fmt == '\0') {
            *fmt = NULL; // all done
            break;
        }
        if (**fmt == '%') {
            switch (*(*fmt + 1)) {
                case 'd':
                case 'i':
                    int i = va_arg(arg, int);
                    break;
                case 'u':
                    unsigned int u = va_arg(arg, unsigned int);
                    break;
                case 'X':
                case 'x':
                    unsigned int x = va_arg(arg, unsigned int);
                    // print hex, whithout 0x
                    break;
                case 'p':
                    void *p = va_arg(arg, void *);
                    // print in hex with 0x
                    break;
                case 'c':
                    char c = va_arg(arg, char);
                    // print character
                    break;
                case 's':
                    const char* s = va_arg(arg, char *);
                    // print string
                    break;
                case 'a':
                case 'A':
                case 'e':
                case 'E':
                case 'f':
                case 'F':
                case 'g':
                case 'G':
                    double d = va_arg(arg, double);
                    (void) d;
                    break;
                case 'o':
                    unsigned int o = va_arg(arg, unsigned int);
                    (void) o;
                    break;
                case 'n':
                    int* n = va_arg(arg, int*);
                    (void) n;
                    break;
                case '%':   /* fall through */
                default:
                    //print character
                    break;
            }

        }
        if (*(*fmt + 1) == 'x') {
            if (remaining_size >= HEX_PRINT_MIN_SZ) {
                (*fmt) += 2;
                // int x = (int)va_arg(arg, int);
                // // int cnt = hex_to_str(buf, x);
                // int cnt = 1;
                // remaining_size -= cnt;
                // buf += cnt;
            }
            continue;
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
