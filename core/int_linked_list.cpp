


#include "int_linked_list.h"


IntLinkedList::Cursor::Cursor() : node_(0) {}


bool IntLinkedList::Cursor::hasNext() const { return node_ != 0; }


int IntLinkedList::Cursor::next()
{
    int value = node_->value;
    node_ = node_->next;
    return value;
}


IntLinkedList::Cursor::Cursor(const Node* start) : node_(start) {}


IntLinkedList::IntLinkedList() : head_(0), tail_(0), size_(0) {}


IntLinkedList::IntLinkedList(const IntLinkedList& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}


IntLinkedList& IntLinkedList::operator=(const IntLinkedList& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}


IntLinkedList::~IntLinkedList() { clear(); }


void IntLinkedList::pushBack(int value)
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


void IntLinkedList::pushFront(int value)
{
    Node* node = new Node;
    node->value = value;
    node->next = head_;
    head_ = node;
    if (tail_ == 0)
        tail_ = node;
    ++size_;
}


bool IntLinkedList::popFront(int& outValue)
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


bool IntLinkedList::isEmpty() const { return size_ == 0; }


int IntLinkedList::size() const { return size_; }


bool IntLinkedList::contains(int value) const
{
    for (Node* n = head_; n != 0; n = n->next)
        if (n->value == value)
            return true;
    return false;
}


bool IntLinkedList::removeValue(int value)
{
    Node* prev = 0;
    for (Node* n = head_; n != 0; prev = n, n = n->next) {
        if (n->value == value) {
            if (prev == 0)
                head_ = n->next;
            else
                prev->next = n->next;
            if (tail_ == n)
                tail_ = prev;
            delete n;
            --size_;
            return true;
        }
    }
    return false;
}


void IntLinkedList::clear()
{
    while (head_ != 0) {
        Node* dead = head_;
        head_ = head_->next;
        delete dead;
    }
    tail_ = 0;
    size_ = 0;
}


IntLinkedList::Cursor IntLinkedList::cursor() const
{
    return Cursor(head_);
}


void IntLinkedList::copyFrom(const IntLinkedList& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        pushBack(n->value);
}
