#pragma once

#include <atomic>
#include <type_traits>

namespace lockfree {


template <typename T>
class SPSCQueue2
{
  public:
    struct SPSCNode
    {
        SPSCNode ()
          : next (nullptr)
        {

        }

        Node * next;
    };

    static_assert (
        std::is_base_of<SPSCNode, T>::value,
        "Type must be of SPSCNode"
    );

  private:
    std::atomic<T *> head_;  // shared
    T * tail_;   // consumer only

  public:


    SPSCQueue2 ()
    {
        tail_ = head_ = new T ();
    }

    ~SPSCQueue2 ()
    {
        while (tail_ != nullptr)
        {
          // release the list
          Node * tmp = tail_;
          tail_ = tmp->next;
          delete tmp;
        }
    }

    void push (const T node)
    {
        head_->next = node;
        head_ = node;
    }

    bool pop (T & result)
    {
        T * tail = tail_;
        T * next = tail->next;

        if (next == nullptr)
        {
            return false;
        }

        T * nextnext = next->next;

        // means next != head_, will not clobber next pointer
        if (nextnext)
        {
            result = next;
            tail_->next_ = nextnext;
            return true;
        }

        // next == head_, carefully swap head pointer.
        // There is a chance tail_ next got upda


        return false;               // else report empty
    }
};

} // namespace lockfree