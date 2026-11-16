#pragma once
// IntLinkedQueue — the project's own FIFO queue of int values, used by BFS to
// hold the frontier of discovered cells. Standalone DSA implementation:
// no templates, no <queue>. Self-contained nodes with head_/tail_ pointers
// give O(1) enqueue and dequeue.
// Ownership: the queue owns every node; clear() and the destructor free them.

class IntLinkedQueue {
private:
    // One chain link: the stored value plus a link to the next node.
    struct Node {
        int value;
        Node* next;
    };

public:
    IntLinkedQueue();                          // empty queue
    IntLinkedQueue(const IntLinkedQueue& other);             // deep copy
    IntLinkedQueue& operator=(const IntLinkedQueue& other);  // deep copy
    ~IntLinkedQueue();                         // frees every node

    void enqueue(int value);                   // insert at the tail, O(1)
    bool dequeue(int& outValue);               // remove head; false when empty
    bool isEmpty() const;                      // true when size is zero
    int size() const;                          // number of waiting values
    void clear();                              // delete all nodes

private:
    Node* head_;   // dequeue here (oldest value)
    Node* tail_;   // enqueue here (newest value)
    int size_;     // node count

    void copyFrom(const IntLinkedQueue& other); // helper: deep-copy 'other' nodes
};
