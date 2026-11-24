


#include "string_list.h"


StringList::StringList() : head_(0), tail_(0), size_(0) {}


StringList::StringList(const StringList& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}


StringList& StringList::operator=(const StringList& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}


StringList::~StringList() { clear(); }


void StringList::pushBack(const std::string& value)
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


bool StringList::popFront(std::string& outValue)
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


bool StringList::isEmpty() const { return size_ == 0; }


int StringList::size() const { return size_; }


void StringList::clear()
{
    while (head_ != 0) {
        Node* dead = head_;
        head_ = head_->next;
        delete dead;
    }
    tail_ = 0;
    size_ = 0;
}


void StringList::copyFrom(const StringList& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        pushBack(n->value);
}
