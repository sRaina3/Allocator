#include <algorithm> // count
#include <cstddef>   // ptrdiff_t
#include <string>    // string

#include <gtest/gtest.h> // gtest

#include "Allocator.hpp"

using namespace std;

struct A {
    friend bool operator == (const A&, const A&) {
        A::log += "==(A, A) ";
        return true;
    }

    static string log;

    A  ()         = default;
    A  (int)      {
        log += "A(int) ";
    }
    A  (const A&) {
        log += "A(A) ";
    }
    ~A ()         {
        log += "~A() ";
    }
};

string A::log;

TEST(AllocatorFixture, test0) {
    using allocator_type = my_allocator<A, 1000>;
    using value_type     = typename allocator_type::value_type;
    using size_type      = typename allocator_type::size_type;
    using pointer        = typename allocator_type::pointer;

    A::log.clear();

    allocator_type  x;
    const size_type s = 2;
    const pointer   b = x.allocate(s);

    const value_type v = 0;
    ASSERT_EQ(A::log, "A(int) ");

    const pointer e = b + s;
    pointer p = b;
    while (p != e) {
        x.construct(p, v);
        ++p;
    }
    ASSERT_EQ(A::log, "A(int) A(A) A(A) ");

    ASSERT_EQ(count(b, e, v), ptrdiff_t(s));
    ASSERT_EQ(A::log, "A(int) A(A) A(A) ==(A, A) ==(A, A) ");

    p = e;
    while (b != p) {
        --p;
        x.destroy(p);
    }
    ASSERT_EQ(A::log, "A(int) A(A) A(A) ==(A, A) ==(A, A) ~A() ~A() ");
    x.deallocate(b, s);
}

TEST(AllocatorFixture, test1) {
    using allocator_type = my_allocator<A, 1000>;

    allocator_type x;        // read/write
    ASSERT_EQ(x[  0], 992);
    ASSERT_EQ(x[996], 992);
}

TEST(AllocatorFixture, test2) {
    using allocator_type = my_allocator<A, 1000>;

    const allocator_type x;  // read-only
    ASSERT_EQ(x[  0], 992);
    ASSERT_EQ(x[996], 992);
}

// Added Tests
// Allocation/Deallocation Tests
TEST(AllocatorFixture, test3) {
    using allocator_type = my_allocator<double, 1000>;
    allocator_type x;

    x.allocate(50);
    ASSERT_EQ(x[  0], -400);
    ASSERT_EQ(x[408], 584);
}

TEST(AllocatorFixture, test4) {
    using allocator_type = my_allocator<double, 1000>;
    using pointer        = typename allocator_type::pointer;
    allocator_type x;

    pointer b = x.allocate(5);
    x.deallocate(b, 0);
    ASSERT_EQ(x[  0], 992);
    ASSERT_EQ(x[996], 992);
}

TEST(AllocatorFixture, test5) {
    using allocator_type = my_allocator<double, 1000>;
    using pointer        = typename allocator_type::pointer;
    allocator_type x;

    pointer b = x.allocate(5);
    x.allocate(1);
    x.deallocate(b, 0);
    ASSERT_EQ(x[  0], 40);
    ASSERT_EQ(x[48], -8);
    ASSERT_EQ(x[64], 928);
}

TEST(AllocatorFixture, test6) {
    using allocator_type = my_allocator<double, 1000>;
    using pointer        = typename allocator_type::pointer;
    allocator_type x;

    pointer b = x.allocate(5);
    pointer c = x.allocate(1);
    x.deallocate(b, 0);
    x.deallocate(c, 0);
    ASSERT_EQ(x[  0], 992);
    ASSERT_EQ(x[996], 992);
}

TEST(AllocatorFixture, test7) {
    using allocator_type = my_allocator<double, 1000>;
    using pointer        = typename allocator_type::pointer;
    allocator_type x;

    pointer b = x.allocate(2);
    x.allocate(2);
    pointer c = x.allocate(2);
    x.deallocate(b, 0);
    x.deallocate(c, 0);
    ASSERT_EQ(x[  0], 16);
    ASSERT_EQ(x[24], -16);
    ASSERT_EQ(x[48], 944);
}

TEST(AllocatorFixture, test8) {
    using allocator_type = my_allocator<double, 1000>;
    using pointer        = typename allocator_type::pointer;
    allocator_type x;

    pointer b = x.allocate(2);
    pointer c = x.allocate(2);
    pointer d = x.allocate(2);
    x.deallocate(b, 0);
    x.deallocate(d, 0);
    x.deallocate(c, 0);
    ASSERT_EQ(x[  0], 992);
    ASSERT_EQ(x[996], 992);
}

TEST(AllocatorFixture, test9) {
    using allocator_type = my_allocator<double, 1000>;
    allocator_type x;

    x.allocate(3);
    x.allocate(2);
    ASSERT_EQ(x[0], -24);
    ASSERT_EQ(x[32], -16);
    ASSERT_EQ(x[56], 936);
}

TEST(AllocatorFixture, test10) {
    using allocator_type = my_allocator<double, 1000>;
    using pointer        = typename allocator_type::pointer;
    allocator_type x;

    x.allocate(1);
    pointer b = x.allocate(10);
    x.deallocate(b,0);

    ASSERT_EQ(x[  0], -8);
    ASSERT_EQ(x[16], 976);
}

// Const Iterator test
TEST(AllocatorFixture, test11) {
    using allocator_type = my_allocator<double, 1000>;
    const allocator_type x;

    allocator_type::const_iterator iter = x.begin();
    allocator_type::const_iterator iter_end = x.end();
    ++iter;
    iter_end--;
    iter_end++;
    ASSERT_EQ(*iter, *iter_end);
}

// Iterator tests
TEST(AllocatorFixture, test12) {
    using allocator_type = my_allocator<double, 1000>;
    allocator_type x;

    x.allocate(5);
    allocator_type::iterator iter = x.begin();
    ASSERT_EQ(*iter, -40);
}

TEST(AllocatorFixture, test13) {
    using allocator_type = my_allocator<double, 1000>;
    allocator_type x;

    x.allocate(5);
    allocator_type::iterator iter = x.begin();
    ++iter;
    ASSERT_EQ(*iter, 944);
}

TEST(AllocatorFixture, test14) {
    using allocator_type = my_allocator<double, 1000>;
    allocator_type x;

    x.allocate(5);
    allocator_type::iterator iter = x.begin();
    allocator_type::iterator iter_end = x.end();
    ++iter;
    iter++;
    ASSERT_EQ(*iter, *iter_end);
}

TEST(AllocatorFixture, test15) {
    using allocator_type = my_allocator<double, 1000>;
    using pointer        = typename allocator_type::pointer;
    allocator_type x;

    pointer b = x.allocate(5);
    x.deallocate(b, 0);
    allocator_type::iterator iter = x.begin();
    allocator_type::iterator iter_end = x.end();
    ASSERT_EQ(*iter, 992);
}

TEST(AllocatorFixture, test16) {
    using allocator_type = my_allocator<double, 1000>;
    using pointer        = typename allocator_type::pointer;
    allocator_type x;

    pointer b = x.allocate(5);
    x.deallocate(b, 0);
    allocator_type::iterator iter = x.begin();
    allocator_type::iterator iter_end = x.end();
    ++iter;
    ASSERT_EQ(*iter, *iter_end);
}

TEST(AllocatorFixture, test17) {
    using allocator_type = my_allocator<double, 1000>;
    using pointer        = typename allocator_type::pointer;
    allocator_type x;

    allocator_type::iterator iter = x.begin();
    allocator_type::iterator iter_end = x.end();
    iter_end--;
    ASSERT_EQ(*iter, *iter_end);
}

TEST(AllocatorFixture, test18) {
    using allocator_type = my_allocator<double, 1000>;
    using pointer        = typename allocator_type::pointer;
    allocator_type x;

    x.allocate(20);
    allocator_type::iterator iter_end = x.end();
    --iter_end;
    ASSERT_EQ(*iter_end, x[168]);
}

TEST(AllocatorFixture, test19) {
    using allocator_type = my_allocator<double, 1000>;
    using pointer        = typename allocator_type::pointer;
    allocator_type x;

    x.allocate(20);
    allocator_type::iterator iter = x.begin();
    allocator_type::iterator iter_end = x.end();
    --iter_end;
    --iter_end;
    ASSERT_EQ(*iter_end, x[0]);
    ASSERT_EQ(*iter, *iter_end);
}
