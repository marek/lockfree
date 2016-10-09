#define CACHE_SIZE 64

template <typename T>
class MPSCQueue
{
    static_assert(
        std::is_pointer<T>::value,
        "Type must be a pointer."
    );

  private:
    struct Node
    {
        T value;
        mpscq_node_t*  volatile  next;
    };

    __attribute__((aligned(CACHE_SIZE))) atomic<Node *> head_;
    __attribute__((aligned(CACHE_SIZE))) mpscq_node_t * tail_;
    __attribute__((aligned(CACHE_SIZE))) mpscq_node_t stub_;

  public:
    MPSCQueue ()
    {
        head_ = new Node ();
        head_.next = nullptr;
        tail_ = head_;
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

    void Push (T value)
    {
        Node * n = new Node ();
        n.value = value;
        n.next = nullptr;

        Node * prev = head_.atomic_exchange (
            n,
            std::memory_order_acq_rel
        );

        prev->next = n;
    }

    bool tryPop (T & value)
    {
        //
        // We aways want to keep one node on the queue
        // So use node's value as an extra check to see if it's being used.
        //
        Node * tail = tail_;
        Node * next = tail->next;
        value = nullptr;

        if (tail.value != nullptr)
        {
            value = tail.value;
            tail.value = nullptr;
        }

        if (next == nullptr)
        {
            // No nodes after us
            // Keep tail, return result status
            return value != nullptr;
        }

        tail_ = next;
        delete tail;
        return true;

    }

    inline T pop()
    {
        T ret;
        while (!tryPop (ret))
        {
            // Empty
        }
        return ret;
    }

};


