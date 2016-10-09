#pragma once

#include <atomic>

namespace lockfree {
//
// Based on http://www.drdobbs.com/parallel/writing-lock-free-code-a-corrected-queue/210604448?pgno=2
//
template <typename T>
class SPSCQueue
{
    static_assert(
        std::is_pointer<T>::value,
        "Type must be a pointer."
    );

  private:
    struct Node
    {
        Node( T val )
          : value (val),
            next (nullptr)
        {

        }

        T value;
        Node * next;
    };
    Node * head_;                          // for producer only
    std::atomic<Node *> divider_, tail_;   // shared

  public:
    SPSCQueue ()
    {
        head_ = divider_ = tail_ = new Node( T() );   // add dummy separator
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

    void push (const T & value)
    {
        auto tail = tail_.load ();
        tail->next = new Node(value);     // add the new item
        tail_  = tail->next;              // publish it

        while( head_ != divider_ )
        {
            // trim unused nodes
            Node * tmp = head_;
            head_ = head_->next;
            delete tmp;
        }
    }

    bool pop ( T & result )
    {
        auto divider = divider_.load ();
        if ( divider != tail_ )
        {
            // if queue is nonempty
            result = divider->next->value;    // C: copy it back
            divider_ = divider->next;        // D: publish that we took it
            return true;                      // and report success
        }
        return false;               // else report empty
    }
};

} // namespace lockfree