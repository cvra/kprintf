#include <stdint.h>
#include <stdarg.h>
#include <string.h>

// convert to hexadecimal (with leading 0s)
// buffer must be 8+1 bytes long
// return length of string (without null character)
static int itoa_hex(uint32_t x, char *buffer)
{
    static const char hex_tbl[]
        = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    char *w = buffer + 8;
    *w = '\0';
    do {
        *--w = hex_tbl[x & 0x0f];
        x >>= 4;
    } while(w > buffer);
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

int vfkprintf(int (*writefn)(void *arg, const char *str, int len),
    void *p, const char *fmt, va_list arg)
{
    int count = 0;
    char str_buf[sizeof("-2147483647")];
    while (1) {
        int i = 0;
        while (fmt[i] != '\0' && fmt[i] != '%') {
            i++;
        }

        if (i > 0) {
            /* write string */
            count += writefn(p, fmt, i);
            fmt += i;
        }

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
                int len = writefn(p, str_buf, slen);
                if (len != slen) {
                    return -1;
                }
                count += len;
                break;
            }
            case 'u': { /* unsigned int */
                unsigned int u = va_arg(arg, unsigned int);
                int slen = utoa_dec((uint32_t)u, str_buf);
                int len = writefn(p, str_buf, slen);
                if (len != slen) {
                    return -1;
                }
                count += len;
                break;
            }
            case 'p': /* pointer */
            case 'X': /* unsigned int in hexacedimal */
            case 'x': {
                unsigned int x = va_arg(arg, unsigned int);
                int slen = itoa_hex((uint32_t)x, str_buf);
                int len = writefn(p, str_buf, slen);
                if (len != slen) {
                    return -1;
                }
                count += len;
                break;
            }
            case 'c': { /* character */
                char c = (char) va_arg(arg, int);
                count += writefn(p, &c, 1);
                break;
            }
            case 's': { /* string */
                const char* s = va_arg(arg, char *);
                int slen = strlen(s);
                count += writefn(p, s, slen);
                break;
            }
            case '%': /* print '%' character */
                count += writefn(p, fmt, 1);
                break;
            default: /* not supported format specifier, abort */
                return count;
        }
        fmt++;
    }
    return count;
}


int kprintf(int (*writefn)(void *arg, const char *str, int len),
    void *p, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = vfkprintf(writefn, p, fmt, args);
    va_end(args);
    return ret;
}

struct snkprintf_buf {
    int size;
    char *str;
};

static int snkprintf_write(void *arg, const char *str, int len)
{
    struct snkprintf_buf *buf = (struct snkprintf_buf *)arg;
    int count;
    if (len > buf->size) {
        count = buf->size;
        buf->size = 0;
    } else {
        count = len;
        buf->size -= len;
    }
    while (count-- > 0) {
        *buf->str++ = *str++;
    }
    return len;
}

int snkprintf(char *str, size_t size, const char *fmt, ...)
{
    struct snkprintf_buf buf = {.str = str, .size = (int)size - 1};

    va_list args;
    va_start(args, fmt);
    int ret = vfkprintf(snkprintf_write, &buf, fmt, args);
    va_end(args);

    *buf.str = '\0';
    return ret;
}
