// SolveStepList implementation. Same shape as CarveStepList but for BFS
// animation records; steps are appended during solve and replayed by the GUI.

#include "solve_step_list.h"

// An unused cursor walks nothing: hasNext() is false from the start.
SolveStepList::Cursor::Cursor() : node_(0) {}

// A cursor is exhausted when it no longer points at a node.
bool SolveStepList::Cursor::hasNext() const { return node_ != 0; }

// Return the step under the cursor and advance one node.
const SolveStep& SolveStepList::Cursor::next()
{
    const SolveStep& step = node_->step;
    node_ = node_->next;
    return step;
}

// Private constructor used by cursor(): begin a walk at 'start'.
SolveStepList::Cursor::Cursor(const Node* start) : node_(start) {}

// An empty list has no nodes and size zero.
SolveStepList::SolveStepList() : head_(0), tail_(0), size_(0) {}

// Copy constructor: deep-copies the other list's steps, node by node.
SolveStepList::SolveStepList(const SolveStepList& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}

// Copy assignment: free this list first, then deep-copy the other list.
SolveStepList& SolveStepList::operator=(const SolveStepList& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}

// Destructor: frees every node in recorded order.
SolveStepList::~SolveStepList() { clear(); }

// Append one recorded BFS action at the tail in O(1).
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

// True when no steps were recorded.
bool SolveStepList::isEmpty() const { return size_ == 0; }

// Number of recorded steps.
int SolveStepList::size() const { return size_; }

// Delete every recorded step, leaving an empty list.
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

// Start a read-only walk at the first recorded step.
SolveStepList::Cursor SolveStepList::cursor() const
{
    return Cursor(head_);
}

// Helper for copy construction/assignment: append copies of the other nodes.
void SolveStepList::copyFrom(const SolveStepList& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        pushBack(n->step);
}
