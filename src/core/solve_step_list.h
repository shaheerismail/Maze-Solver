#pragma once
// SolveStepList — the project's own singly linked list of SolveStep records.
// BfsSolver appends one record per dequeue / edge check; the GUI replays the
// list to animate the search. Standalone DSA implementation: no templates.
// Ownership: the list owns every node; clear() and the destructor free them.

#include "steps.h"

class SolveStepList {
private:
    // One chain link: the stored step plus a link to the next node.
    struct Node {
        SolveStep step;
        Node* next;
    };

public:
    // Read-only forward cursor: hasNext() asks if more remain, next()
    // returns the current step and advances one node.
    class Cursor {
    public:
        Cursor();                            // empty cursor: hasNext() is false
        bool hasNext() const;                // true while a node is pending
        const SolveStep& next();             // current step, then advance
    private:
        friend class SolveStepList;
        explicit Cursor(const Node* start);  // start walking at 'start'
        const Node* node_;
    };

    SolveStepList();                         // empty list
    SolveStepList(const SolveStepList& other);             // deep copy
    SolveStepList& operator=(const SolveStepList& other);  // deep copy
    ~SolveStepList();                        // frees every node

    void pushBack(const SolveStep& step);    // append at the tail, O(1)
    bool isEmpty() const;                    // true when size is zero
    int size() const;                        // number of stored steps
    void clear();                            // delete all nodes
    Cursor cursor() const;                   // start a read-only walk

private:
    Node* head_;   // first recorded step
    Node* tail_;   // last recorded step (keeps pushBack O(1))
    int size_;     // node count

    void copyFrom(const SolveStepList& other); // helper: deep-copy 'other' nodes
};
