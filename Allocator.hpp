#ifndef Allocator_hpp
#define Allocator_hpp

#include <cassert>   // assert
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument
#include <iostream>  // cout

template <typename T, std::size_t N>
class my_allocator {
    friend bool operator == (const my_allocator&, const my_allocator&) {
        return false;
    }                                            

    friend bool operator != (const my_allocator& lhs, const my_allocator& rhs) {
        return !(lhs == rhs);
    }

public:
    using      value_type = T;

    using       size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using       pointer   =       value_type*;
    using const_pointer   = const value_type*;

    using       reference =       value_type&;
    using const_reference = const value_type&;

public:
    class iterator {
        /* Compares two iterators and determines if they point to same
          location
          Input: Two const iterators
          Output: True if iterators at same point, False otherwise */
        friend bool operator == (const iterator& lhs, const iterator& rhs) {
            // Check addresses of iterator pointers
            if (&(*lhs) != &(*rhs)) {
                return false;
            }
            return true;
        } 

        friend bool operator != (const iterator& lhs, const iterator& rhs) {
            return !(lhs == rhs);
        }

    private:
        int* _p;

    public:
        iterator (int* p) {
            _p = p;
        }
        // Return value at iterator pointer
        int& operator * () const {
            return *_p;
        }           
 
        /* Moves over iterator to next block, returns moved over
           iterator */
        iterator& operator ++ () {
            int size = *(*this);
            assert(abs(size) >= 0);
            // Go to next block
            this->_p += abs(size)/4 + 2;
            return *this;
        }

        iterator operator ++ (int) {
            iterator x = *this;
            ++*this;
            return x;
        }

        /* Moves over iterator to previous block, returns moved over
           iterator */
        iterator& operator -- () {
            // Go to end sentinel of previous block
            this->_p -= 1;
            int size = *(*this);
            assert(abs(size) >= 0);
            // Go to start of previous block
            this->_p -= abs(size)/4 + 1;
            return *this;
        }

        iterator operator -- (int) {
            iterator x = *this;
            --*this;
            return x;
        }
    };

    class const_iterator {

        /* Compares two iterators and determines if they point to same
          location
          Input: Two const iterators
          Output: True if iterators at same point, False otherwise */
        friend bool operator == (const const_iterator& lhs, const const_iterator& rhs) {
            // Check addresses of iterator pointers
            if (&(*lhs) != &(*rhs)) {
                return false;
            }
            return true;
        }                

        friend bool operator != (const const_iterator& lhs, const const_iterator& rhs) {
            return !(lhs == rhs);
        }

    private:
        const int* _p;

    public:
        const_iterator (const int* p) {
            _p = p;
        }

        // Return value at iterator pointer
        const int& operator * () const {
            return *_p;
        }           

        /* Moves over iterator to next block, returns moved over
           iterator */
        const_iterator& operator ++ () {
            int size = *(*this);
            assert(abs(size) >= 0);
            // Go to next block
            this->_p += abs(size)/4 + 2;
            return *this;
        }

        const_iterator operator ++ (int) {
            const_iterator x = *this;
            ++*this;
            return x;
        }

        /* Moves over iterator to previous block, returns moved over
           iterator */
        const_iterator& operator -- () {
            // Go to end sentinel of previous block
            this->_p -= 1;
            int size = *(*this);
            assert(abs(size) >= 0);
            // Go to start of previous block
            this->_p -= abs(size)/4 + 1;
            return *this;
        }

        const_iterator operator -- (int) {
            const_iterator x = *this;
            --*this;
            return x;
        }
    };

private:
    char a[N];
    /**
     * O(1) in space
     * O(n) in time
     * Checks if array is valid, meaning that each beginning and end
     * sentinel contain same size and are located at beginning and end of
     * each block
     * Output: True if valid conditions satisfied, false otherwise
     */
    bool valid () const {
        const_iterator c_iter = begin();
        const_iterator end_iter = end();
        while (c_iter != end_iter) {
            // Size held by Beginning sentinel
            int size = abs(*c_iter);
            assert(size >= 0);
            const int* p = &(*c_iter);
            // Ensure End sentinel holds same size
            p += size/4 + 1;
            int end_size = abs(*p);
            if (end_size != size) {
                return false;
            }
            ++c_iter;
        }
        return true;
    }

public:
    /**
     * O(1) in space
     * O(1) in time
     * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
     */
    my_allocator () {
        (*this)[0]   = N-8;
        (*this)[N-4] = N-8;
        assert(valid());
    }

    my_allocator             (const my_allocator&) = default;
    ~my_allocator            ()                    = default;
    my_allocator& operator = (const my_allocator&) = default;

    /**
     * O(1) in space
     * O(n) in time
     * after allocation there must be enough space left for a valid block
     * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
     * choose the first block that fits
     * throw a bad_alloc exception, if n is invalid
     */
    pointer allocate (size_type s) {
        assert(s > 0);
        iterator iter = begin();
        iterator iter_end = end();
        int* p;
        bool found = false;
        int sc = (int)s;
        // check each block
        while (iter != iter_end && !found) {
            // get curr block size
            int size = *iter;
            if (size >= sc*8) {
                p = &(*iter);
                found = true;
                // update current sentinel
                *p = -(sc*8);
                p += s*2 + 1;
                *p = -(sc*8);
                int remaining = size - (sc*8 + 8);
                // update new sentinel for remaining block
                if (remaining >= 0) {
                    p += 1;
                    *p = remaining;
                    p += remaining/4 + 1;
                    *p = remaining;
                }
            } else {
                ++iter;
            }
        }
        if (!found) {
            std::bad_alloc exception;
            throw exception;
        }
        assert(valid());
        return (T*)(&(*iter));
    }

    /**
     * O(1) in space
     * O(1) in time
     */
    void construct (pointer p, const_reference v) {
        new (p) T(v);                         
        assert(valid());
    }                 

    /**
     * O(1) in space
     * O(1) in time
     * after deallocation adjacent free blocks must be coalesced
     * throw an invalid_argument exception, if p is invalid
     * Input: p, a pointer to the start of the block to be deallocated
     * Output: None, but block must be deallocated and coalesced with
     * adjacent blocks if they are also free
     */
    void deallocate (pointer p, size_type) const {
        int* pc = (int *)p;
        // get current block size
        int size = -1 * *pc;
        assert(size >= 0);
        // update block's sentinels to deallocated
        *pc = size;
        pc += size/4 + 1;
        *pc = size;
        int* coalescer = (int *)p;
        const_iterator c_iter = begin();
        // Ensure that deallocated block isn't the first block
        if (&(*c_iter) != coalescer) {
            coalescer -= 1;
            int prev_size = *coalescer;
            // Check if previous block is also free
            if (prev_size >= 0) {
                coalescer -= prev_size/4 + 1;
                int new_size = prev_size + size + 8;
                // Update prev block beginning sentinel
                *coalescer = new_size;
                coalescer += new_size/4 + 1;
                // Update current block end sentinel, combines both blocks
                *coalescer = new_size;
            }
        }

        int* f_coal = (int *)p;
        f_coal += size/4 + 1;
        int old_size = *f_coal;
        f_coal += 1;
        c_iter = end();
        // Ensure that deallocated block isn't the last block
        if (&(*c_iter) > f_coal) {
            int next_size = *f_coal;
            // Check if next block is also free
            if (next_size >= 0) {
                f_coal += next_size/4 + 1;
                int new_size = old_size + next_size + 8;
                // Update current block beginning sentinel
                *f_coal = new_size;
                f_coal -= new_size/4 + 1;
                // Update next block end sentinel, combines both blocks
                *f_coal = new_size;
            }
        }
        assert(valid());
    }

    /**
     * O(1) in space
     * O(1) in time
     */
    void destroy (pointer p) {
        p->~T();            
        assert(valid());
    }

    /**
     * O(1) in space
     * O(1) in time
     */
    int& operator [] (int i) {
        return *reinterpret_cast<int*>(&a[i]);
    }

    /**
     * O(1) in space
     * O(1) in time
     */
    const int& operator [] (int i) const {
        return *reinterpret_cast<const int*>(&a[i]);
    }

    /**
     * O(1) in space
     * O(1) in time
     */
    iterator begin () {
        return iterator(&(*this)[0]);
    }

    /**
     * O(1) in space
     * O(1) in time
     */
    const_iterator begin () const {
        return const_iterator(&(*this)[0]);
    }

    /**
     * O(1) in space
     * O(1) in time
     */
    iterator end () {
        return iterator(&(*this)[N]);
    }

    /**
     * O(1) in space
     * O(1) in time
     */
    const_iterator end () const {
        return const_iterator(&(*this)[N]);
    }
};

#endif // Allocator_hpp

