// IntLinkedQueue implementation. FIFO order is guaranteed by enqueueing at
// tail_ and dequeuing at head_. Each new pairs with exactly one delete.

#include "int_linked_queue.h"

// An empty queue has no nodes and size zero.
IntLinkedQueue::IntLinkedQueue() : head_(0), tail_(0), size_(0) {}

// Copy constructor: deep-copies the other queue's nodes in FIFO order.
IntLinkedQueue::IntLinkedQueue(const IntLinkedQueue& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}

// Copy assignment: free this queue first, then deep-copy the other queue.
IntLinkedQueue& IntLinkedQueue::operator=(const IntLinkedQueue& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}

// Destructor: frees every node, oldest first.
IntLinkedQueue::~IntLinkedQueue() { clear(); }

// Insert a value at the tail in O(1), preserving arrival order.
void IntLinkedQueue::enqueue(int value)
{
    Node* node = new Node;
    node->value = value;
    node->next = 0;
    if (tail_ != 0)
        tail_->next = node;
    else
        head_ = node;
    tail_ = node;
    ++size_;
}

// Remove the oldest value into 'outValue'; returns false when the queue is empty.
bool IntLinkedQueue::dequeue(int& outValue)
{
    if (head_ == 0)
        return false;
    outValue = head_->value;
    Node* dead = head_;
    head_ = head_->next;
    if (head_ == 0)
        tail_ = 0;
    delete dead;
    --size_;
    return true;
}

// True when the queue holds no waiting values.
bool IntLinkedQueue::isEmpty() const { return size_ == 0; }

// Number of waiting values.
int IntLinkedQueue::size() const { return size_; }

// Delete every node, leaving an empty queue (BFS calls this before each solve).
void IntLinkedQueue::clear()
{
    while (head_ != 0) {
        Node* dead = head_;
        head_ = head_->next;
        delete dead;
    }
    tail_ = 0;
    size_ = 0;
}

// Helper for copy construction/assignment: enqueue copies of the other nodes.
void IntLinkedQueue::copyFrom(const IntLinkedQueue& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        enqueue(n->value);
}
