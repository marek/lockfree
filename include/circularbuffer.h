#pragma once

#include <atomic>
#include <cassert>
#include <unistd.h>
#include <type_traits>

namespace lockfree {

#define CACHE_SIZE 64

template<typename T>
class CircularBuffer
{
    static_assert (
        std::is_pointer<T>::value,
        "Type must be a pointer."
    );

private:
    alignas (CACHE_SIZE) std::atomic<uint64_t> high_;
    alignas (CACHE_SIZE) std::atomic<uint64_t> low_;
    alignas (CACHE_SIZE) size_t capacity_;
    alignas (CACHE_SIZE) T * buffer_;

public:
    explicit CircularBuffer (size_t capacity)
        : high_ {0},
          low_ {0},
          capacity_ {capacity}
    {
        assert (capacity_);
        buffer_ = new T[capacity_]();
    }

    ~CircularBuffer()
    {
        delete [] buffer_;
    }

    //
    // No copying or assignment
    //
    CircularBuffer (const CircularBuffer&) = delete;
    CircularBuffer& operator= (const CircularBuffer&) = delete;

    inline bool tryPush (const T in)
    {
        assert(in);

        const uint64_t low = low_.load (std::memory_order_relaxed);
        uint64_t high = high_.load (std::memory_order_acquire);
        const uint64_t index = high % capacity_;
        if (high - low < capacity_
            && std::atomic_compare_exchange_strong_explicit (
                &high_,
                &high,
                high + 1,
                std::memory_order_release, std::memory_order_relaxed
            ))
        {
            buffer_[index] = in;
            return true;
        }
        return false;
    }

    inline void push (const T in)
    {
        while (! tryPush (in))
        {
            // Empty
        }
    }

    inline T tryPop()
    {
        const uint64_t high = high_.load (std::memory_order_relaxed);
        uint64_t low = low_.load (std::memory_order_acquire);
        const uint64_t index = low % capacity_;
        T const ret = const_cast<T> (buffer_[index]);
        if (ret
            && high > low
            && std::atomic_compare_exchange_strong_explicit (
                &low_, &low, low + 1,
                std::memory_order_relaxed, std::memory_order_relaxed
            ))
        {
            return ret;
        }
        return nullptr;
    }

    inline T pop ()
    {
        T ret;
        while (!(ret = tryPop ()))
        {
            // Empty
        }
        return ret;
    }

    inline size_t size ()
    {
        const uint64_t high = high_.load (std::memory_order_relaxed);
        const int64_t size = high - low_.load (std::memory_order_acquire);
        return size >= 0 ? size : 0;
    }
};

} // namespace lockfree
