#pragma once
// MazeRecordList — the project's own singly linked list of MazeRecord entries.
// MazeLibrary::inOrder fills it with the BST's records sorted by seed, and the
// GUI walks the result. Standalone DSA implementation: no templates.
// Ownership: the list owns every node (and a copy of each record inside);
// clear() and the destructor free them.

#include "steps.h"

class MazeRecordList {
private:
    // One chain link: the stored record plus a link to the next node.
    struct Node {
        MazeRecord record;
        Node* next;
    };

public:
    // Read-only forward cursor: hasNext() asks if more remain, next()
    // returns the current record and advances one node.
    class Cursor {
    public:
        Cursor();                            // empty cursor: hasNext() is false
        bool hasNext() const;                // true while a node is pending
        const MazeRecord& next();            // current record, then advance
    private:
        friend class MazeRecordList;
        explicit Cursor(const Node* start);  // start walking at 'start'
        const Node* node_;
    };

    MazeRecordList();                        // empty list
    MazeRecordList(const MazeRecordList& other);             // deep copy
    MazeRecordList& operator=(const MazeRecordList& other);  // deep copy
    ~MazeRecordList();                       // frees every node

    void pushBack(const MazeRecord& record); // append at the tail, O(1)
    bool isEmpty() const;                    // true when size is zero
    int size() const;                        // number of stored records
    void clear();                            // delete all nodes
    Cursor cursor() const;                   // start a read-only walk

private:
    Node* head_;   // first record
    Node* tail_;   // last record (keeps pushBack O(1))
    int size_;     // node count

    void copyFrom(const MazeRecordList& other); // helper: deep-copy 'other' nodes
};
