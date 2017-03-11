#pragma once

#include <queue>
#include <atomic>

#define CACHE_SIZE 64

namespace lockfree {

//
// Simpler version of spsc_queue.h
// without the ability of pooling the internal nodes
//

template <typename T>
class SPSCQueue
{
    static_assert (
        std::is_pointer<T>::value,
        "Type must be a pointer."
    );

  private:
    struct Node
    {
        Node ( T val = nullptr)
          : value {val},
            next {nullptr}
        {

        }

        T value;
        Node * next;
    };

    alignas (CACHE_SIZE) std::atomic<Node *> head_; // shared
    alignas (CACHE_SIZE) std::atomic<Node *> tail_;    // shared

  public:
    SPSCQueue ()
    {
        head_ = tail_ = new Node ();  // add dummy separator
    }

    ~SPSCQueue ()
    {
        while (head_ != nullptr)
        {
          // release the list
          Node * tmp = head_;
          head_ = tmp->next;
          delete tmp;
        }
    }

    //
    // No copying or assignment
    //
    SPSCQueue (const SPSCQueue&) = delete;
    SPSCQueue& operator= (const SPSCQueue&) = delete;

    void push (const T & value)
    {
        auto tail = tail_.load ();
        tail->next = new Node (value);     // add the new item
        tail_  = tail->next;                // publish it
    }

    bool tryPop ( T & result )
    {
        auto head = head_.load ();
        if (head_ != tail_)
        {
            // if queue is nonempty
            result = head->next->value;    // C: copy it back
            head_ = head->next;         // D: publish that we took it
            return true;                      // and report success
        }
        return false;               // else report empty
    }

    T && pop ()
    {
        T result;
        while (! tryPop (result))
        {

        }
        return std::move (result);
    }
};

} // namespace lockfree