


#include "int_linked_queue.h"


IntLinkedQueue::IntLinkedQueue() : head_(0), tail_(0), size_(0) {}


IntLinkedQueue::IntLinkedQueue(const IntLinkedQueue& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}


IntLinkedQueue& IntLinkedQueue::operator=(const IntLinkedQueue& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}


IntLinkedQueue::~IntLinkedQueue() { clear(); }


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


bool IntLinkedQueue::isEmpty() const { return size_ == 0; }


int IntLinkedQueue::size() const { return size_; }


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


void IntLinkedQueue::copyFrom(const IntLinkedQueue& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        enqueue(n->value);
}
