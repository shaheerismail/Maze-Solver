#pragma once
// IntLinkedList — the project's own singly linked list of int values.
// Standalone DSA implementation (no templates, no <vector>): it stores cell ids
// for graph adjacency lists, BFS routes, visited cells, mines, keys and trails.
// Ownership: the list owns every node; clear() and the destructor free them.

class IntLinkedList {
private:
    // One chain link: the stored value plus a link to the next node.
    struct Node {
        int value;
        Node* next;
    };

public:
    // Read-only forward cursor: hasNext() asks if more remain, next()
    // returns the current value and advances one node.
    class Cursor {
    public:
        Cursor();                            // empty cursor: hasNext() is false
        bool hasNext() const;                // true while a node is pending
        int next();                          // current value, then advance
    private:
        friend class IntLinkedList;
        explicit Cursor(const Node* start);  // start walking at 'start'
        const Node* node_;
    };

    IntLinkedList();                         // empty list
    IntLinkedList(const IntLinkedList& other);             // deep copy
    IntLinkedList& operator=(const IntLinkedList& other);  // deep copy
    ~IntLinkedList();                        // frees every node

    void pushBack(int value);                // append at the tail, O(1)
    void pushFront(int value);               // prepend at the head, O(1)
    bool popFront(int& outValue);            // remove head; false when empty
    bool isEmpty() const;                    // true when size is zero
    int size() const;                        // number of stored values
    bool contains(int value) const;          // linear scan for an equal value
    bool removeValue(int value);             // unlink the FIRST matching node
    void clear();                            // delete all nodes
    Cursor cursor() const;                   // start a read-only walk

private:
    Node* head_;   // front of the list (popFront here)
    Node* tail_;   // back of the list (pushBack here, keeps append O(1))
    int size_;     // node count

    void copyFrom(const IntLinkedList& other); // helper: deep-copy 'other' nodes
};
