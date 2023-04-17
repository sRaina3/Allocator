#include <iostream>         // cin, cout

#include <cassert>          // assert

#include <sstream>          // stringstream

#include <algorithm> // count

#include <cstddef>   // ptrdiff_t

#include <vector>           // vector

#include "Allocator.hpp"

using namespace std;

int main () {
    using allocator_type = my_allocator<double, 1000>;
    using value_type     = typename allocator_type::value_type;
    using size_type      = typename allocator_type::size_type;
    using pointer        = typename allocator_type::pointer;

    int tests;
    cin >> tests;
    assert(tests > 0);
    assert(tests <= 100);
    string skip;
    getline(cin, skip);
    getline(cin, skip);
    // Iterate through each test
    for (int i = 0; i < tests; ++i) {
        allocator_type x;
        // Tracks beginning of all allocated blocks
        vector<pointer> blocks;
        string request;
        // Read in each input for current test set
        while (getline(cin, request) && request.length() != 0) {
            stringstream ss(request);
            int temp;
            ss >> temp;
            assert(temp != 0);
            // Allocate for positive requests
            if (temp > 0) {
                pointer b = x.allocate(temp);
                blocks.push_back(b);
                sort(blocks.begin(), blocks.end());
                // Deallocate for negative requests
            } else {
                x.deallocate(blocks[(-temp) - 1], 0);
                // Remove block from list of allocated blocks
                blocks.erase(blocks.begin() + ((-temp) - 1));
            }
        }
        allocator_type::iterator iter = x.begin();
        allocator_type::iterator iter_end = x.end();
        // Iterate through and print out value of every beginning sentinel
        while (iter != iter_end) {
            cout << *iter;
            ++iter;
            if (iter != iter_end) {
                cout << " ";
            }
        }
        cout << '\n';
        assert(iter == iter_end);
    }
    return 0;
}
