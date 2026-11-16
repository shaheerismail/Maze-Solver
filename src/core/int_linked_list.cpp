// IntLinkedList implementation. Every function is O(n) at worst; append and
// prepend are O(1) thanks to the tail_ pointer. Each new pairs with one delete.

#include "int_linked_list.h"

// An unused cursor walks nothing: hasNext() is false from the start.
IntLinkedList::Cursor::Cursor() : node_(0) {}

// A cursor is exhausted when it no longer points at a node.
bool IntLinkedList::Cursor::hasNext() const { return node_ != 0; }

// Return the value under the cursor and advance one node.
int IntLinkedList::Cursor::next()
{
    int value = node_->value;
    node_ = node_->next;
    return value;
}

// Private constructor used by cursor(): begin a walk at 'start'.
IntLinkedList::Cursor::Cursor(const Node* start) : node_(start) {}

// An empty list has no nodes and size zero.
IntLinkedList::IntLinkedList() : head_(0), tail_(0), size_(0) {}

// Copy constructor: deep-copies the other list's nodes, node by node.
IntLinkedList::IntLinkedList(const IntLinkedList& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}

// Copy assignment: free this list first, then deep-copy the other list.
IntLinkedList& IntLinkedList::operator=(const IntLinkedList& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}

// Destructor: frees every node in front-to-back order.
IntLinkedList::~IntLinkedList() { clear(); }

// Append a value at the tail in O(1) via the tail_ pointer.
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

// Prepend a value at the head in O(1) (used to build BFS routes back to front).
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

// Remove the head node into 'outValue'; returns false when the list is empty.
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

// True when the list holds no nodes.
bool IntLinkedList::isEmpty() const { return size_ == 0; }

// Number of stored values.
int IntLinkedList::size() const { return size_; }

// Linear scan: true when an equal value is stored anywhere in the list.
bool IntLinkedList::contains(int value) const
{
    for (Node* n = head_; n != 0; n = n->next)
        if (n->value == value)
            return true;
    return false;
}

// Unlink and delete the FIRST node holding an equal value; false if absent.
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

// Delete every node, leaving an empty list (safe to call on an empty list).
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

// Start a read-only walk at the head of the list.
IntLinkedList::Cursor IntLinkedList::cursor() const
{
    return Cursor(head_);
}

// Helper for copy construction/assignment: append copies of the other nodes.
void IntLinkedList::copyFrom(const IntLinkedList& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        pushBack(n->value);
}
