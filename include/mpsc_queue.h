#pragma once

#include <atomic>
#include <cassert>
#include <type_traits>

namespace lockfree {

#define CACHE_SIZE 64

template <typename T>
class MPSCQueue
{
    static_assert (
        std::is_pointer<T>::value,
        "Type must be a pointer."
    );

  private:
    struct Node
    {
        Node ()
        : value {nullptr},
          next {nullptr}
        {

        }
        T value;
        Node*  volatile  next;
    };

    alignas (CACHE_SIZE) std::atomic<Node *> head_;
    alignas (CACHE_SIZE) Node * tail_;

  public:
    MPSCQueue ()
    {
        auto head = new Node ();
        head_ = tail_ = head;
    }

    ~MPSCQueue ()
    {
        Node * n = tail_;
        while (n != nullptr)
        {
            Node * next = n->next;
            delete n;
            n = next;
        }
    }

    void push (T value)
    {
        assert (value);

        Node * n = new Node ();
        n->value = value;

        Node * prev = head_.exchange (
            n,
            std::memory_order_acq_rel
        );

        prev->next = n;
    }

    bool tryPop (T & value)
    {
        //
        // We aways want to keep one node on the queue
        //
        Node * tail = tail_;
        Node * next = tail->next;
        value = nullptr;

        //
        // tail never has a value.
        // The next one in line does.

        if (next == nullptr)
        {
            // No nodes after us. We need to leave at least one
            // on the queue. Return the result's success status
            return false;
        }

        //
        // Otherwise we have more nodes! yay!
        //
        value = next->value;
        next->value = nullptr;
        tail_ = next;

        delete tail;

        return true;
    }

    inline T pop ()
    {
        T ret;
        while (!tryPop (ret))
        {
            // Empty
        }
        return ret;
    }
};

} // namespace lockfree
