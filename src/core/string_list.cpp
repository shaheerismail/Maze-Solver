// StringList implementation. Nodes own their std::string, so deleting a node
// releases its text automatically; the list itself frees node by node.

#include "string_list.h"

// An empty list has no nodes and size zero.
StringList::StringList() : head_(0), tail_(0), size_(0) {}

// Copy constructor: deep-copies the other list's strings, node by node.
StringList::StringList(const StringList& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}

// Copy assignment: free this list first, then deep-copy the other list.
StringList& StringList::operator=(const StringList& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}

// Destructor: frees every node and the string each one holds.
StringList::~StringList() { clear(); }

// Append one string at the tail in O(1).
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

// Remove the head string into 'outValue'; returns false when the list is empty.
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

// True when the list holds no strings.
bool StringList::isEmpty() const { return size_ == 0; }

// Number of stored strings.
int StringList::size() const { return size_; }

// Delete every node, leaving an empty list.
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

// Helper for copy construction/assignment: append copies of the other nodes.
void StringList::copyFrom(const StringList& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        pushBack(n->value);
}
