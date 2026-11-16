#pragma once
// CarveStepList — the project's own singly linked list of CarveStep records.
// MazeGenerator appends every carved wall to it; the GUI replays the list to
// animate the carving. Standalone DSA implementation: no templates.
// Ownership: the list owns every node; clear() and the destructor free them.

#include "steps.h"

class CarveStepList {
private:
    // One chain link: the stored step plus a link to the next node.
    struct Node {
        CarveStep step;
        Node* next;
    };

public:
    // Read-only forward cursor: hasNext() asks if more remain, next()
    // returns the current step and advances one node.
    class Cursor {
    public:
        Cursor();                            // empty cursor: hasNext() is false
        bool hasNext() const;                // true while a node is pending
        const CarveStep& next();             // current step, then advance
    private:
        friend class CarveStepList;
        explicit Cursor(const Node* start);  // start walking at 'start'
        const Node* node_;
    };

    CarveStepList();                         // empty list
    CarveStepList(const CarveStepList& other);             // deep copy
    CarveStepList& operator=(const CarveStepList& other);  // deep copy
    ~CarveStepList();                        // frees every node

    void pushBack(const CarveStep& step);    // append at the tail, O(1)
    bool isEmpty() const;                    // true when size is zero
    int size() const;                        // number of stored steps
    void clear();                            // delete all nodes
    Cursor cursor() const;                   // start a read-only walk

private:
    Node* head_;   // first recorded step
    Node* tail_;   // last recorded step (keeps pushBack O(1))
    int size_;     // node count

    void copyFrom(const CarveStepList& other); // helper: deep-copy 'other' nodes
};
