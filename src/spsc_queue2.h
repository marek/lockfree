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
        while (head_ != nullptr)
        {
          // release the list
          Node * tmp = head_;
          head_ = tmp->next;
          delete tmp;
        }
    }

    void push (const T node)
    {
        node->next = head_;
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

        result = next;
        tail_->next = next->next_;


        return false;               // else report empty
    }
};

} // namespace lockfree