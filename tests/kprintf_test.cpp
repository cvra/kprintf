#include <string.h>
#include <stdint.h>
#include <limits.h>
#include "CppUTest/TestHarness.h"
#include "../kprintf.h"

char buffer[1024];
int write_pos;

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
    }
};

TEST(KprintfTestGroup, EmptyString)
{
    const char *str = "";
    int ret = kprintf(test_write, str);
    CHECK_EQUAL(strlen(str), ret);
    STRCMP_EQUAL(str, buffer);
}

TEST(KprintfTestGroup, NoFormatArguments)
{
    const char *str = "hello world";
    int ret = kprintf(test_write, str);
    CHECK_EQUAL(strlen(str), ret);
    STRCMP_EQUAL(str, buffer);
}

TEST(KprintfTestGroup, CanPrintIntMax)
{
    int32_t i = INT_MAX;
    int ret = kprintf(test_write, "%d", i);
    const char *expect = "2147483647";
    CHECK_EQUAL(strlen(expect), ret);
    STRCMP_EQUAL(expect, buffer);
}

TEST(KprintfTestGroup, CanPrintIntMin)
{
    int32_t i = INT_MIN;
    int ret = kprintf(test_write, "%d", i);
    const char *expect = "-2147483648";
    CHECK_EQUAL(strlen(expect), ret);
    STRCMP_EQUAL(expect, buffer);
}

TEST(KprintfTestGroup, CanPrintUintMax)
{
    uint32_t i = UINT_MAX;
    int ret = kprintf(test_write, "%u", i);
    const char *expect = "4294967295";
    CHECK_EQUAL(strlen(expect), ret);
    STRCMP_EQUAL(expect, buffer);
}

TEST(KprintfTestGroup, CanPrintHex)
{
    int32_t i = 0x1234abcd;
    int ret = kprintf(test_write, "0x%x", i);
    const char *expect = "0x1234abcd";
    CHECK_EQUAL(strlen(expect), ret);
    STRCMP_EQUAL(expect, buffer);
}

TEST(KprintfTestGroup, CanPrintPtr)
{
    uint32_t p = 0x12ab;
    int ret = kprintf(test_write, "%p", p);
    const char *expect = "000012ab";
    CHECK_EQUAL(strlen(expect), ret);
    STRCMP_EQUAL(expect, buffer);
}

TEST(KprintfTestGroup, CanPrintMultipleArgs)
{
    uint32_t p = 0xc0ffee42;
    int i = 42;
    char s[] = "hello";
    int ret = kprintf(test_write, "int: %d, pointer: %p, string: %s, char: %c", i, p, s, 'c');
    const char *expect = "int: 42, pointer: c0ffee42, string: hello, char: c";
    CHECK_EQUAL(strlen(expect), ret);
    STRCMP_EQUAL(expect, buffer);
}

TEST(KprintfTestGroup, UnsupportedTypes)
{
    /* unsupported format specifiers: f, F, e, E, g, G, a, A, n, o */
    int ret = kprintf(test_write, "hello%f %F %e %E %g %G %a %A %n %o %d",
        .1, .1, .1, .1, .1, .1, .1, .1, NULL, 0, 42);
    const char *expect = "hello";
    CHECK_EQUAL(strlen(expect), ret);
    STRCMP_EQUAL(expect, buffer);
}

TEST(KprintfTestGroup, CANPrintPercent)
{
    int ret = kprintf(test_write, "%%");
    CHECK_EQUAL(1, ret);
    STRCMP_EQUAL("%", buffer);
}

TEST(KprintfTestGroup, IncompleteFormatSpecifier)
{
    int ret = kprintf(test_write, "%");
    CHECK_EQUAL(0, ret);
    STRCMP_EQUAL("", buffer);
}
