


#include "solve_step_list.h"


SolveStepList::Cursor::Cursor() : node_(0) {}


bool SolveStepList::Cursor::hasNext() const { return node_ != 0; }


const SolveStep& SolveStepList::Cursor::next()
{
    const SolveStep& step = node_->step;
    node_ = node_->next;
    return step;
}


SolveStepList::Cursor::Cursor(const Node* start) : node_(start) {}


SolveStepList::SolveStepList() : head_(0), tail_(0), size_(0) {}


SolveStepList::SolveStepList(const SolveStepList& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}


SolveStepList& SolveStepList::operator=(const SolveStepList& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}


SolveStepList::~SolveStepList() { clear(); }


void SolveStepList::pushBack(const SolveStep& step)
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


bool SolveStepList::isEmpty() const { return size_ == 0; }


int SolveStepList::size() const { return size_; }


void SolveStepList::clear()
{
    while (head_ != 0) {
        Node* dead = head_;
        head_ = head_->next;
        delete dead;
    }
    tail_ = 0;
    size_ = 0;
}


SolveStepList::Cursor SolveStepList::cursor() const
{
    return Cursor(head_);
}


void SolveStepList::copyFrom(const SolveStepList& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        pushBack(n->step);
}
