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

