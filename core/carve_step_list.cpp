


#include "carve_step_list.h"


CarveStepList::Cursor::Cursor() : node_(0) {}


bool CarveStepList::Cursor::hasNext() const { return node_ != 0; }


const CarveStep& CarveStepList::Cursor::next()
{
    const CarveStep& step = node_->step;
    node_ = node_->next;
    return step;
}


CarveStepList::Cursor::Cursor(const Node* start) : node_(start) {}


CarveStepList::CarveStepList() : head_(0), tail_(0), size_(0) {}


CarveStepList::CarveStepList(const CarveStepList& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}


CarveStepList& CarveStepList::operator=(const CarveStepList& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}


CarveStepList::~CarveStepList() { clear(); }


void CarveStepList::pushBack(const CarveStep& step)
{
    Node* node = new Node;
    node->step = step;
    node->next = 0;
    if (tail_ != 0)
        tail_->next = node;
    else
        head_ = node;
    tail_ = node;
    ++size_;
}


bool CarveStepList::isEmpty() const { return size_ == 0; }


int CarveStepList::size() const { return size_; }


void CarveStepList::clear()
{
    while (head_ != 0) {
        Node* dead = head_;
        head_ = head_->next;
        delete dead;
    }
    tail_ = 0;
    size_ = 0;
}


CarveStepList::Cursor CarveStepList::cursor() const
{
    return Cursor(head_);
}


void CarveStepList::copyFrom(const CarveStepList& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        pushBack(n->step);
}
