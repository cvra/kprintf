#include <stdint.h>
#include <stdarg.h>
#include <string.h>

typedef int (*kprintf_write_fn_t) (const char *buf, int len);

kprintf_write_fn_t kprintf_stdout = NULL;

#define TO_STRING_BUFFER_SIZE 12

static char itoa_hex_digit(uint8_t n)
{
    n = n & 0x0F;
    if (n > 0x09) {
        return 'a' + n-10;
    } else {
        return '0' + n;
    }
}

// convert to hexadecimal (with leading 0s)
// buffer must be 8+1 bytes long
// return length of string (without null character)
static int itoa_hex(uint32_t x, char *buffer)
{
    char *w = buffer;
    int i;
    for (i = 32-4; i >= 0; i -= 4) {
        *w++ = itoa_hex_digit(x>>i);
    }
    *w = '\0';
    return 8;
}

// convert to decimal
// buffer must be 10+1 chars long for full range
// returns lenght of string (without null character)
static int utoa_dec(uint32_t x, char *buffer)
{
    int len = 0;
    char *a = buffer;
    char *b = a;
    do {
        *b++ = '0' + x%10;
        x /= 10;
        len++;
    } while (x > 0);
    *b = '\0';
    while (b > a) { // swap the digits a...b
        char tmp = *a;
        *a++ = *--b;
        *b = tmp;
    }
    return len;
}

// convert to decimal
// buffer must be 11+1 chars long for full range
// returns lenght of string (without null character)
static int itoa_dec(int32_t x, char *buffer)
{
    int len = 0;
    if (x < 0) {
        x = -x;
        *buffer++ = '-';
        len++;
    }
    len += utoa_dec(x, buffer);
    return len;
}

int vfkprintf(kprintf_write_fn_t writefn, const char *fmt, va_list arg)
{
    int count = 0;
    char str_buf[TO_STRING_BUFFER_SIZE];
    while (1) {
        int i = 0;
        while (fmt[i] != '\0' && fmt[i] != '%') {
            i++;
        }

        if (i > 0) {
            /* write string */
            int len = writefn(fmt, i);
            if (len != i) {
                return -1;
            } else {
                count += len;
            }
        }

        fmt += i;

        /* end of string */
        if (*fmt == '\0') {
            break;
        }

        /* handle format specifier */
        fmt++;
        switch (*fmt) {
            case 'd':
            case 'i': { /* int */
                int i = va_arg(arg, int);
                int slen = itoa_dec((int32_t)i, str_buf);
                int len = writefn(str_buf, slen);
                if (len != slen) {
                    return -1;
                }
                count += len;
                break;
            }
            case 'u': { /* unsigned int */
                unsigned int u = va_arg(arg, unsigned int);
                int slen = utoa_dec((uint32_t)u, str_buf);
                int len = writefn(str_buf, slen);
                if (len != slen) {
                    return -1;
                }
                count += len;
                break;
            }
            case 'X':
            case 'x': { /* unsigned int in hexacedimal */
                unsigned int x = va_arg(arg, unsigned int);
                int slen = itoa_hex((uint32_t)x, str_buf);
                int len = writefn(str_buf, slen);
                if (len != slen) {
                    return -1;
                }
                count += len;
                break;
            }
            case 'o': { /* unsigned int in octal */
                unsigned int o = va_arg(arg, unsigned int);
                (void) o;   // ignored
                break;
            }
            case 'p': { /* pointer */
                void *p = va_arg(arg, void *);
                str_buf[0] = '0';
                str_buf[1] = 'x';
                int slen = 2 + itoa_hex((size_t)p, str_buf + 2);
                int len = writefn(str_buf, slen);
                if (len != slen) {
                    return -1;
                }
                count += len;
                break;
            }
            case 'c': { /* character */
                char c = (char) va_arg(arg, int);
                int len = writefn(&c, 1);
                if (len != 1) {
                    return -1;
                }
                count += len;
                break;
            }
            case 's': { /* string */
                const char* s = va_arg(arg, char *);
                int slen = strlen(s);
                int len = writefn(s, slen);
                if (len != slen) {
                    return -1;
                }
                count += len;
                break;
            }
            case 'a':
            case 'A':
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G': { /* double */
                double d = va_arg(arg, double);
                (void) d;   // ignored
                break;
            }
            case 'n': { /* return character count */
                int *n = va_arg(arg, int *);
                (void) n;   // ignored
                break;
            }
            case '%': {
                // print '%' character
                int len = writefn(fmt, 1);
                if (len != 1) {
                    return -1;
                }
                count += len;
                break;
            }
            case '\0': /* incomplete format specifier */
                // ignore and return normally
                return count;
            default:    // ignored
                break;
        }
        fmt++;
    }
    return count;
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
