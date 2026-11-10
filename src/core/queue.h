#pragma once
// LinkedQueue<T> — the FIFO queue used by BFS. Self-contained nodes with
// head_/tail_ pointers give O(1) enqueue and dequeue. Header-only template.
//
// Ownership: the queue owns every node; clear() and the destructor free them.

template <typename T>
class LinkedQueue {
public:
    LinkedQueue() : head_(0), tail_(0), size_(0) {}

    LinkedQueue(const LinkedQueue& other) : head_(0), tail_(0), size_(0)
    {
        *this = other;
    }

    LinkedQueue& operator=(const LinkedQueue& other)
    {
        if (this != &other) {
            clear();
            for (QNode* n = other.head_; n != 0; n = n->next)
                enqueue(n->value);
        }
        return *this;
    }

    ~LinkedQueue() { clear(); }

    void enqueue(const T& value)
    {
        QNode* node = new QNode;
        node->value = value;
        node->next = 0;
        if (tail_ != 0) {
            tail_->next = node;
        } else {
            head_ = node;
        }
        tail_ = node;
        ++size_;
    }

    bool dequeue(T& out)
    {
        if (head_ == 0)
            return false;
        out = head_->value;
        QNode* dead = head_;
        head_ = head_->next;
        if (head_ == 0)
            tail_ = 0;
        delete dead;
        --size_;
        return true;
    }

    bool isEmpty() const { return size_ == 0; }
    int size() const { return size_; }

    void clear()
    {
        while (head_ != 0) {
            QNode* dead = head_;
            head_ = head_->next;
            delete dead;
        }
        tail_ = 0;
        size_ = 0;
    }

private:
    struct QNode {
        T value;
        QNode* next;
    };

    QNode* head_;   // dequeue here
    QNode* tail_;   // enqueue here
    int size_;
};
