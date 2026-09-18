#pragma once

#include <string>

namespace ov4
{

template <typename T>
class T_raw_mem
{

public:

    void *_p;
    std::size_t _len;

    explicit T_raw_mem(std::size_t size)
    {
        _p = operator new(size);
        _len = size;
    }

    ~T_raw_mem()
    {
        operator delete(_p);
    }

    T *get() const noexcept
    {
        return static_cast<T*>(_p);
    }

    std::size_t get_size() const noexcept
    {
        return len;
    }

    // COPY is not allowed
    T_raw_mem(const T_raw_mem&) = delete;
    T_raw_mem& operator=(const T_raw_mem&) = delete;

    // MOVE constructor
    T_raw_mem(T_raw_mem &&x) noexcept
        : p(std::exchange(x._p, nullptr)), slen(std::exchange(x._len, 0))
    { /* empty */ }

    T_raw_mem& operator=(T_raw_mem &&x) noexcept
    {
        // if they are same
        // release itself, which is not expected
        if (this != &x)
        {
            // consider statement: y = x;
            // release memory of y (if exist)
            operator delete(_p);

            // and then move
            _p = std::exchange(x._p, nullptr);
            _len = std::exchange(x._len, 0);
        }

        return *this;
    }

};

}
