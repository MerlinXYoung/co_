#include "co/unitest.h"
#include "co/fast.h"
#include <cstdio>

namespace test {

DEF_test(fast) {
    char buf[24];

    DEF_case(u32toa) {
        EXPECT_EQ(fastring(buf, fast::u32toa(0, buf)), "0");
        EXPECT_EQ(fastring(buf, fast::u32toa(1, buf)), "1");
        EXPECT_EQ(fastring(buf, fast::u32toa(12, buf)), "12");
        EXPECT_EQ(fastring(buf, fast::u32toa(123, buf)), "123");
        EXPECT_EQ(fastring(buf, fast::u32toa(1234, buf)), "1234");
        EXPECT_EQ(fastring(buf, fast::u32toa(12345, buf)), "12345");
        EXPECT_EQ(fastring(buf, fast::u32toa(123456, buf)), "123456");
        EXPECT_EQ(fastring(buf, fast::u32toa(1234567, buf)), "1234567");
        EXPECT_EQ(fastring(buf, fast::u32toa(12345678, buf)), "12345678");
        EXPECT_EQ(fastring(buf, fast::u32toa(123456789, buf)), "123456789");
        EXPECT_EQ(fastring(buf, fast::u32toa(1234567890, buf)), "1234567890");
        EXPECT_EQ(fastring(buf, fast::u32toa(3234567890U, buf)), "3234567890");
    }

    DEF_case(i32toa) {
        EXPECT_EQ(fastring(buf, fast::i32toa(0, buf)), "0");
        EXPECT_EQ(fastring(buf, fast::i32toa(1, buf)), "1");
        EXPECT_EQ(fastring(buf, fast::i32toa(-9, buf)), "-9");
        EXPECT_EQ(fastring(buf, fast::i32toa(1234567, buf)), "1234567");
        EXPECT_EQ(fastring(buf, fast::i32toa(-12345678, buf)), "-12345678");
        EXPECT_EQ(fastring(buf, fast::i32toa(-123456789, buf)), "-123456789");
    }

    DEF_case(u64toa) {
        EXPECT_EQ(fastring(buf, fast::u64toa(0, buf)), "0");
        EXPECT_EQ(fastring(buf, fast::u64toa(9, buf)), "9");
        EXPECT_EQ(fastring(buf, fast::u64toa(999, buf)), "999");
        EXPECT_EQ(fastring(buf, fast::u64toa(1234567, buf)), "1234567");
        EXPECT_EQ(fastring(buf, fast::u64toa(12345678, buf)), "12345678");
        EXPECT_EQ(fastring(buf, fast::u64toa(123456789, buf)), "123456789");
        EXPECT_EQ(fastring(buf, fast::u64toa(1234567890, buf)), "1234567890");
        EXPECT_EQ(fastring(buf, fast::u64toa(123456789012ULL, buf)), "123456789012");
        EXPECT_EQ(fastring(buf, fast::u64toa(12345678901234ULL, buf)), "12345678901234");
        EXPECT_EQ(fastring(buf, fast::u64toa(123456789012345ULL, buf)), "123456789012345");
        EXPECT_EQ(fastring(buf, fast::u64toa(1234567890123456ULL, buf)), "1234567890123456");
        EXPECT_EQ(fastring(buf, fast::u64toa(12345678901234567ULL, buf)), "12345678901234567");
        EXPECT_EQ(fastring(buf, fast::u64toa(123456789012345678ULL, buf)), "123456789012345678");
        EXPECT_EQ(fastring(buf, fast::u64toa(1234567890123456789ULL, buf)), "1234567890123456789");
        EXPECT_EQ(fastring(buf, fast::u64toa(12345678901234567890ULL, buf)), "12345678901234567890");
    }

    DEF_case(i64toa) {
        EXPECT_EQ(fastring(buf, fast::i64toa(0, buf)), "0");
        EXPECT_EQ(fastring(buf, fast::i64toa(9, buf)), "9");
        EXPECT_EQ(fastring(buf, fast::i64toa(-9, buf)), "-9");
        EXPECT_EQ(fastring(buf, fast::i64toa(1234567, buf)), "1234567");
        EXPECT_EQ(fastring(buf, fast::i64toa(-12345678, buf)), "-12345678");
        EXPECT_EQ(fastring(buf, fast::i64toa(-123456789, buf)), "-123456789");
        EXPECT_EQ(fastring(buf, fast::i64toa(1234567890, buf)), "1234567890");
        EXPECT_EQ(fastring(buf, fast::i64toa(123456789012ULL, buf)), "123456789012");
        EXPECT_EQ(fastring(buf, fast::i64toa(12345678901234ULL, buf)), "12345678901234");
        EXPECT_EQ(fastring(buf, fast::i64toa(-123456789012345LL, buf)), "-123456789012345");
        EXPECT_EQ(fastring(buf, fast::i64toa(1234567890123456ULL, buf)), "1234567890123456");
        EXPECT_EQ(fastring(buf, fast::i64toa(12345678901234567ULL, buf)), "12345678901234567");
        EXPECT_EQ(fastring(buf, fast::i64toa(123456789012345678ULL, buf)), "123456789012345678");
        EXPECT_EQ(fastring(buf, fast::i64toa(-1234567890123456789LL, buf)), "-1234567890123456789");
    }

    DEF_case(u32toh_with_prefix) {
        EXPECT_EQ(fastring(buf, fast::u32toh_with_prefix(1, buf)), "0x1");
        EXPECT_EQ(fastring(buf, fast::u32toh_with_prefix(0xa, buf)), "0xa");
        EXPECT_EQ(fastring(buf, fast::u32toh_with_prefix(0xf, buf)), "0xf");
        EXPECT_EQ(fastring(buf, fast::u32toh_with_prefix(0xff, buf)), "0xff");
        EXPECT_EQ(fastring(buf, fast::u32toh_with_prefix(0x123456, buf)), "0x123456");
        EXPECT_EQ(fastring(buf, fast::u32toh_with_prefix(0x1234567, buf)), "0x1234567");
        EXPECT_EQ(fastring(buf, fast::u32toh_with_prefix(0x12345678, buf)), "0x12345678");
        EXPECT_EQ(fastring(buf, fast::u32toh_with_prefix(0xffffffff, buf)), "0xffffffff");
    }

    DEF_case(u64toh_with_prefix) {
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(1, buf)), "0x1");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0xa, buf)), "0xa");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0xf, buf)), "0xf");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0xff, buf)), "0xff");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0x123456, buf)), "0x123456");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0x1234567, buf)), "0x1234567");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0x12345678, buf)), "0x12345678");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0xffffffff, buf)), "0xffffffff");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0xfffffffffULL, buf)), "0xfffffffff");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0xffffffffffffULL, buf)), "0xffffffffffff");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0xffffffffffffffffULL, buf)), "0xffffffffffffffff");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0x1234567890ULL, buf)), "0x1234567890");
        EXPECT_EQ(fastring(buf, fast::u64toh_with_prefix(0x123456789abcdefULL, buf)), "0x123456789abcdef");
    }

    DEF_case(u32tohex_with_prefix) {
        EXPECT_EQ(fastring(buf, fast::u32tohex_with_prefix(1, buf)), "0x1");
        EXPECT_EQ(fastring(buf, fast::u32tohex_with_prefix(0xA, buf)), "0xA");
        EXPECT_EQ(fastring(buf, fast::u32tohex_with_prefix(0xF, buf)), "0xF");
        EXPECT_EQ(fastring(buf, fast::u32tohex_with_prefix(0xFF, buf)), "0xFF");
        EXPECT_EQ(fastring(buf, fast::u32tohex_with_prefix(0x123456, buf)), "0x123456");
        EXPECT_EQ(fastring(buf, fast::u32tohex_with_prefix(0x1234567, buf)), "0x1234567");
        EXPECT_EQ(fastring(buf, fast::u32tohex_with_prefix(0x12345678, buf)), "0x12345678");
        EXPECT_EQ(fastring(buf, fast::u32tohex_with_prefix(0xFFFFFFFF, buf)), "0xFFFFFFFF");
    }

    DEF_case(u64tohex_with_prefix) {
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(1, buf)), "0x1");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0xA, buf)), "0xA");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0xF, buf)), "0xF");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0xFF, buf)), "0xFF");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0x123456, buf)), "0x123456");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0x1234567, buf)), "0x1234567");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0x12345678, buf)), "0x12345678");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0xFFFFFFFF, buf)), "0xFFFFFFFF");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0xFFFFFFFFFULL, buf)), "0xFFFFFFFFF");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0xFFFFFFFFFFFFULL, buf)), "0xFFFFFFFFFFFF");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0xFFFFFFFFFFFFFFFFULL, buf)), "0xFFFFFFFFFFFFFFFF");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0x1234567890ULL, buf)), "0x1234567890");
        EXPECT_EQ(fastring(buf, fast::u64tohex_with_prefix(0x123456789ABCDEFULL, buf)), "0x123456789ABCDEF");
    }


    DEF_case(u32toh) {
        EXPECT_EQ(fastring(buf, fast::u32toh(1, buf)), "1");
        EXPECT_EQ(fastring(buf, fast::u32toh(0xa, buf)), "a");
        EXPECT_EQ(fastring(buf, fast::u32toh(0xf, buf)), "f");
        EXPECT_EQ(fastring(buf, fast::u32toh(0xff, buf)), "ff");
        EXPECT_EQ(fastring(buf, fast::u32toh(0x123456, buf)), "123456");
        EXPECT_EQ(fastring(buf, fast::u32toh(0x1234567, buf)), "1234567");
        EXPECT_EQ(fastring(buf, fast::u32toh(0x12345678, buf)), "12345678");
        EXPECT_EQ(fastring(buf, fast::u32toh(0xffffffff, buf)), "ffffffff");
    }

    DEF_case(u64toh) {
        EXPECT_EQ(fastring(buf, fast::u64toh(1, buf)), "1");
        EXPECT_EQ(fastring(buf, fast::u64toh(0xa, buf)), "a");
        EXPECT_EQ(fastring(buf, fast::u64toh(0xf, buf)), "f");
        EXPECT_EQ(fastring(buf, fast::u64toh(0xff, buf)), "ff");
        EXPECT_EQ(fastring(buf, fast::u64toh(0x123456, buf)), "123456");
        EXPECT_EQ(fastring(buf, fast::u64toh(0x1234567, buf)), "1234567");
        EXPECT_EQ(fastring(buf, fast::u64toh(0x12345678, buf)), "12345678");
        EXPECT_EQ(fastring(buf, fast::u64toh(0xffffffff, buf)), "ffffffff");
        EXPECT_EQ(fastring(buf, fast::u64toh(0xfffffffffULL, buf)), "fffffffff");
        EXPECT_EQ(fastring(buf, fast::u64toh(0xffffffffffffULL, buf)), "ffffffffffff");
        EXPECT_EQ(fastring(buf, fast::u64toh(0xffffffffffffffffULL, buf)), "ffffffffffffffff");
        EXPECT_EQ(fastring(buf, fast::u64toh(0x1234567890ULL, buf)), "1234567890");
        EXPECT_EQ(fastring(buf, fast::u64toh(0x123456789abcdefULL, buf)), "123456789abcdef");
    }

    DEF_case(u32tohex) {
        EXPECT_EQ(fastring(buf, fast::u32tohex(1, buf)), "1");
        EXPECT_EQ(fastring(buf, fast::u32tohex(0xA, buf)), "A");
        EXPECT_EQ(fastring(buf, fast::u32tohex(0xF, buf)), "F");
        EXPECT_EQ(fastring(buf, fast::u32tohex(0xFF, buf)), "FF");
        EXPECT_EQ(fastring(buf, fast::u32tohex(0x123456, buf)), "123456");
        EXPECT_EQ(fastring(buf, fast::u32tohex(0x1234567, buf)), "1234567");
        EXPECT_EQ(fastring(buf, fast::u32tohex(0x12345678, buf)), "12345678");
        EXPECT_EQ(fastring(buf, fast::u32tohex(0xFFFFFFFF, buf)), "FFFFFFFF");
    }

    DEF_case(u64tohex) {
        EXPECT_EQ(fastring(buf, fast::u64tohex(1, buf)), "1");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0xA, buf)), "A");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0xF, buf)), "F");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0xFF, buf)), "FF");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0x123456, buf)), "123456");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0x1234567, buf)), "1234567");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0x12345678, buf)), "12345678");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0xFFFFFFFF, buf)), "FFFFFFFF");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0xFFFFFFFFFULL, buf)), "FFFFFFFFF");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0xFFFFFFFFFFFFULL, buf)), "FFFFFFFFFFFF");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0xFFFFFFFFFFFFFFFFULL, buf)), "FFFFFFFFFFFFFFFF");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0x1234567890ULL, buf)), "1234567890");
        EXPECT_EQ(fastring(buf, fast::u64tohex(0x123456789ABCDEFULL, buf)), "123456789ABCDEF");
    }

    DEF_case(dtoa) {
        EXPECT_EQ(fastring(buf, fast::dtoa(0.0, buf)), "0.0");
        EXPECT_EQ(fastring(buf, fast::dtoa(0.00, buf)), "0.0");
        EXPECT_EQ(fastring(buf, fast::dtoa(0.01, buf)), "0.01");
        EXPECT_EQ(fastring(buf, fast::dtoa(-0.1, buf)), "-0.1");
        EXPECT_EQ(fastring(buf, fast::dtoa(3.14, buf)), "3.14");
        EXPECT_EQ(fastring(buf, fast::dtoa(3.14159, buf)), "3.14159");
        EXPECT_EQ(fastring(buf, fast::dtoa(3e-23, buf)), "3e-23");
        EXPECT_EQ(fastring(buf, fast::dtoa(3.33e-23, buf)), "3.33e-23");
        EXPECT_EQ(fastring(buf, fast::dtoa(3e23, buf)), "3e23");
        EXPECT_EQ(fastring(buf, fast::dtoa(3.33e23, buf)), "3.33e23");
    }

    DEF_case(printf){
        fastring a;

        sprintf(a, "888");
        EXPECT_EQ(a, "888");

        a.clear();
        sprintf(a, "%%");
        EXPECT_EQ(a, "%");

        a.clear();
        sprintf(a, "%8");
        EXPECT_EQ(a, "%8");

        a.clear();
        sprintf(a, "%d", 123L);
        EXPECT_EQ(a, "123");

        a.clear();
        sprintf(a, "%u", 12345U);
        EXPECT_EQ(a, "12345");

        a.clear();
        sprintf(a, "0x%x", 257U);
        EXPECT_EQ(a, "0x101");

        a.clear();
        sprintf(a, "0x%X", 255U);
        EXPECT_EQ(a, "0xFF");

        a.clear();
        sprintf(a, "%ld", -10000000000LL);
        EXPECT_EQ(a, "-10000000000");

        a.clear();
        sprintf(a, "%lu", 10000000000U);
        EXPECT_EQ(a, "10000000000");

        a.clear();
        sprintf(a, "0x%lx", 0xffffffffff);
        EXPECT_EQ(a, "0xffffffffff");

        a.clear();
        sprintf(a, "0x%lX", 0xffffffffff);
        EXPECT_EQ(a, "0xFFFFFFFFFF");

        a.clear();
        sprintf(a, "%p", &a);

        char buf[256];
        auto len = sprintf(buf, "%p", &a);
        buf[len]=0;
        EXPECT_EQ(a, buf);


        a.clear();
        sprintf(a, "%b", true);
        EXPECT_EQ(a, "true");

        a.clear();
        sprintf(a, "%B", false);
        EXPECT_EQ(a, "FALSE");

        a.clear();
        sprintf(a, "%c", 'b');
        EXPECT_EQ(a, "b");

        a.clear();
        sprintf(a, "%s", "abc");
        EXPECT_EQ(a, "abc");

        a.clear();
        sprintf(a, "%.*s", 5, "1234567");
        EXPECT_EQ(a, "12345");

        a.clear();
        sprintf(a, "%f", 0.001);
        EXPECT_EQ(a, "0.001");

        // a.clear();
        // sprintf(a, "%lf", (double)1.001);
        // EXPECT_EQ(a, "1.001"); 

        a.clear();
        sprintf(a, "%s > %u <0x%x %%% %? %.*s %. %B %dd", "hello", 888, 0x10f1, 5, "12345678", true, 15);
        // sprintf(buf, "%s > %u <0x%x %%% %? %.*s %. %b %dd", "hello", 888, 0x10f1, 5, "12345678", true, 15);

        // std::cout<<a<<endl;
        // std::cout<<buf<<endl;
        EXPECT_EQ(a, "hello > 888 <0x10f1 %% %? 12345 %. TRUE 15d");
        // int d=0x1FFF;
        // int64 l=0x8FFFFFFFFF;
        a.clear();
        a<<"hex:"<<fast::hex(0x1fff)<<" "<<10<<" HEX:"<<fast::HEX(0x8FFFFFFFFF);
        EXPECT_EQ(a, "hex:1fff 10 HEX:8FFFFFFFFF");
    }
}

} // namespace test
