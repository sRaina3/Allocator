#include <algorithm> // count
#include <cstddef>   // ptrdiff_t
#include <string>    // string

#include "gtest/gtest.h"

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
