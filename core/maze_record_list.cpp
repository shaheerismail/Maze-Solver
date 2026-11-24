


#include "maze_record_list.h"


MazeRecordList::Cursor::Cursor() : node_(0) {}


bool MazeRecordList::Cursor::hasNext() const { return node_ != 0; }


const MazeRecord& MazeRecordList::Cursor::next()
{
    const MazeRecord& record = node_->record;
    node_ = node_->next;
    return record;
}


MazeRecordList::Cursor::Cursor(const Node* start) : node_(start) {}


MazeRecordList::MazeRecordList() : head_(0), tail_(0), size_(0) {}


MazeRecordList::MazeRecordList(const MazeRecordList& other) : head_(0), tail_(0), size_(0)
{
    copyFrom(other);
}


MazeRecordList& MazeRecordList::operator=(const MazeRecordList& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}


MazeRecordList::~MazeRecordList() { clear(); }


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


bool MazeRecordList::isEmpty() const { return size_ == 0; }


int MazeRecordList::size() const { return size_; }


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


MazeRecordList::Cursor MazeRecordList::cursor() const
{
    return Cursor(head_);
}


void MazeRecordList::copyFrom(const MazeRecordList& other)
{
    for (Node* n = other.head_; n != 0; n = n->next)
        pushBack(n->record);
}
