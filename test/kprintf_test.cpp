#include "CppUTest/TestHarness.h"
#include "../kprintf.h"

char buffer[1024];
int write_pos;
extern "C"
int test_write(const char *buf, int len)
{
    int i = 0;
    while (i < len) {
        buffer[write_pos + i] = buf[i];
        i++;
    }
    write_pos += i;
    return len;
}

TEST_GROUP(KprintfTestGroup)
{
    void setup(void)
    {
        memset(buffer, 0, sizeof(buffer));
        write_pos = 0;
        kprintf_stdout = test_write;
    }
};

TEST(KprintfTestGroup, EmptyString)
{
    const char *str = "";
    int ret = kprintf(str);
    CHECK_EQUAL(strlen(str), ret);
    STRCMP_EQUAL(str, buffer);
}

TEST(KprintfTestGroup, NoFormatArguments)
{
    const char *str = "hello world";
    int ret = kprintf(str);
    CHECK_EQUAL(strlen(str), ret);
    STRCMP_EQUAL(str, buffer);
}

TEST(KprintfTestGroup, CanPrintInt)
{
    int i = -42;
    int ret = kprintf("%d", i);
    CHECK_EQUAL(3, ret);
    STRCMP_EQUAL("-42", buffer);
}

TEST(KprintfTestGroup, CanPrintIntMax)
{
    int i = (1<<31);
    int ret = kprintf("%d", i);
    CHECK_EQUAL(11, ret);   // 10 digits + sign
    STRCMP_EQUAL("-2147483648", buffer);
}

TEST(KprintfTestGroup, CanPrintUint)
{
    int i = 42;
    int ret = kprintf("%u", i);
    CHECK_EQUAL(2, ret);
    STRCMP_EQUAL("42", buffer);
}

TEST(KprintfTestGroup, CanPrintHex)
{
    int i = 0x1234abcd;
    int ret = kprintf("0x%x", i);
    CHECK_EQUAL(10, ret);
    STRCMP_EQUAL("0x1234abcd", buffer);
}

TEST(KprintfTestGroup, CanPrintPtr)
{
    int i = 0x1234abcd;
    int ret = kprintf("%p", i);
    CHECK_EQUAL(10, ret);
    STRCMP_EQUAL("0x1234abcd", buffer);
}

TEST(KprintfTestGroup, CanPrintMultipleArgs)
{
    int p = 0x1234abcd;
    int i = (1<<31);
    char s[] = "hello";
    int ret = kprintf("int: %d, ptr: %p, str: %s, char: %c", i, p, s, 'c');
    const char *expect = "int: -2147483648, ptr: 0x1234abcd, str: hello, char: c";
    CHECK_EQUAL(strlen(expect), ret);
    STRCMP_EQUAL(expect, buffer);
}

TEST(KprintfTestGroup, UnsupportedTypes)
{
    /* unsupported format specifiers: f, F, e, E, g, G, a, A, n, o */
    int ret = kprintf("%f%F%e%E%g%G%a%A%n%o%d",.1,.1,.1,.1,.1,.1,.1,.1,NULL,0,42);
    const char *expect = "42";
    CHECK_EQUAL(strlen(expect), ret);
    STRCMP_EQUAL(expect, buffer);
}

TEST(KprintfTestGroup, IncompleteFormatSpecifier)
{
    int ret = kprintf("%");
    CHECK_EQUAL(0, ret);
    STRCMP_EQUAL("", buffer);
}
