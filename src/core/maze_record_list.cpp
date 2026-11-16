// MazeRecordList implementation. Each node owns a full MazeRecord copy
// (including its walls string); deleting the node releases the string too.

#include "maze_record_list.h"

// An unused cursor walks nothing: hasNext() is false from the start.
MazeRecordList::Cursor::Cursor() : node_(0) {}

// A cursor is exhausted when it no longer points at a node.
bool MazeRecordList::Cursor::hasNext() const { return node_ != 0; }

// Return the record under the cursor and advance one node.
const MazeRecord& MazeRecordList::Cursor::next()
{
    const MazeRecord& record = node_->record;
    node_ = node_->next;
    return record;
}

// Private constructor used by cursor(): begin a walk at 'start'.
MazeRecordList::Cursor::Cursor(const Node* start) : node_(start) {}

// An empty list has no nodes and size zero.
MazeRecordList::MazeRecordList() : head_(0), tail_(0), size_(0) {}

// Copy constructor: deep-copies the other list's records, node by node.
MazeRecordList::MazeRecordList(const MazeRecordList& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}

// Copy assignment: free this list first, then deep-copy the other list.
MazeRecordList& MazeRecordList::operator=(const MazeRecordList& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}

// Destructor: frees every node and the record each one holds.
MazeRecordList::~MazeRecordList() { clear(); }

// Append one maze record at the tail in O(1) (in-order traversal uses this).
void MazeRecordList::pushBack(const MazeRecord& record)
{
    Node* node = new Node;
    node->record = record;
    node->next = 0;
    if (tail_ != 0)
        tail_->next = node;
    else
        head_ = node;
    tail_ = node;
    ++size_;
}

// True when the list holds no records.
bool MazeRecordList::isEmpty() const { return size_ == 0; }

// Number of stored records.
int MazeRecordList::size() const { return size_; }

// Delete every record, leaving an empty list.
void MazeRecordList::clear()
{
    while (head_ != 0) {
        Node* dead = head_;
        head_ = head_->next;
        delete dead;
    }
    tail_ = 0;
    size_ = 0;
}

// Start a read-only walk at the first record.
MazeRecordList::Cursor MazeRecordList::cursor() const
{
    return Cursor(head_);
}

// Helper for copy construction/assignment: append copies of the other nodes.
void MazeRecordList::copyFrom(const MazeRecordList& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        pushBack(n->record);
}
