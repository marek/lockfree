#pragma once

#include <queue>
#include <atomic>

#define CACHE_SIZE 64

namespace lockfree {

//
// Based on Herb Sutter's spsc queue
// http://www.drdobbs.com/parallel/writing-lock-free-code-a-corrected-queue/210604448?pgno=2
//
// This version is better if you want to use a pre-allocated pool of internal
// nodes for performance reasons
// 
// By having the consumer free up used up nodes (hence the added divider)
// You don't have to worry about synchronization of the pool as well.
//
// See spsc_queue.h for a simpler version, that removes this support
//
//

template <typename T>
class HSSPSCQueue
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

    alignas (CACHE_SIZE) Node * head_;                 // for producer only
    alignas (CACHE_SIZE) std::atomic<Node *> divider_; // shared
    alignas (CACHE_SIZE) std::atomic<Node *> tail_;    // shared
    alignas (CACHE_SIZE) std::queue<Node *> freeNodes_;

    Node * allocNode (T val)
    {
        if (! freeNodes_.empty ())
        {
            Node * n = freeNodes_.front ();
            freeNodes_.pop ();
            n->value = val;
            return n;
        }

        return new Node ( val );
    }

    void freeNode (Node * node)
    {
        node->next = nullptr;
        freeNodes_.push (node);
    }

  public:
    HSSPSCQueue (const unsigned cacheSize = 25000)
    {
        head_ = divider_ = tail_ = new Node ();  // add dummy separator

        for (unsigned i = 0; i < cacheSize; ++i)
        {
            freeNodes_.emplace (new Node ());
        }
    }

    ~HSSPSCQueue ()
    {
        while (head_ != nullptr)
        {
          // release the list
          Node * tmp = head_;
          head_ = tmp->next;
          delete tmp;
        }

        while (! freeNodes_.empty ())
        {
            auto node = freeNodes_.front ();
            freeNodes_.pop ();
            delete node;
        }
    }

    //
    // No copying or assignment
    //
    HSSPSCQueue (const HSSPSCQueue&) = delete;
    HSSPSCQueue& operator= (const HSSPSCQueue&) = delete;

    void push (const T & value)
    {
        auto tail = tail_.load ();
        tail->next = allocNode (value);     // add the new item
        tail_  = tail->next;                // publish it

        while (head_ != divider_)
        {
            // trim unused nodes
            Node * tmp = head_;
            head_ = head_->next;
            freeNode (tmp);
        }
    }

    bool tryPop ( T & result )
    {
        auto divider = divider_.load ();
        if (divider != tail_)
        {
            // if queue is nonempty
            result = divider->next->value;    // C: copy it back
            divider_ = divider->next;         // D: publish that we took it
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