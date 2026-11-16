// CarveStepList implementation. Steps are only appended and later replayed in
// order, so pushBack + cursor walking is all the access the project needs.

#include "carve_step_list.h"

// An unused cursor walks nothing: hasNext() is false from the start.
CarveStepList::Cursor::Cursor() : node_(0) {}

// A cursor is exhausted when it no longer points at a node.
bool CarveStepList::Cursor::hasNext() const { return node_ != 0; }

// Return the step under the cursor and advance one node.
const CarveStep& CarveStepList::Cursor::next()
{
    const CarveStep& step = node_->step;
    node_ = node_->next;
    return step;
}

// Private constructor used by cursor(): begin a walk at 'start'.
CarveStepList::Cursor::Cursor(const Node* start) : node_(start) {}

// An empty list has no nodes and size zero.
CarveStepList::CarveStepList() : head_(0), tail_(0), size_(0) {}

// Copy constructor: deep-copies the other list's steps, node by node.
CarveStepList::CarveStepList(const CarveStepList& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}

// Copy assignment: free this list first, then deep-copy the other list.
CarveStepList& CarveStepList::operator=(const CarveStepList& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}

// Destructor: frees every node in recorded order.
CarveStepList::~CarveStepList() { clear(); }

// Append one recorded wall-opening at the tail in O(1).
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

// True when no steps were recorded.
bool CarveStepList::isEmpty() const { return size_ == 0; }

// Number of recorded steps.
int CarveStepList::size() const { return size_; }

// Delete every recorded step, leaving an empty list.
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

// Start a read-only walk at the first recorded step.
CarveStepList::Cursor CarveStepList::cursor() const
{
    return Cursor(head_);
}

// Helper for copy construction/assignment: append copies of the other nodes.
void CarveStepList::copyFrom(const CarveStepList& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        pushBack(n->step);
}
