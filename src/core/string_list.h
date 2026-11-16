#pragma once
// StringList — the project's own singly linked list of std::string values.
// MazeIO uses it to split save-file text into lines. Standalone DSA
// implementation: no templates, no <vector>.
// Ownership: the list owns every node (and the string inside); clear() and
// the destructor free them.

#include <string>

class StringList {
private:
    // One chain link: the stored string plus a link to the next node.
    struct Node {
        std::string value;
        Node* next;
    };

public:
    StringList();                            // empty list
    StringList(const StringList& other);             // deep copy
    StringList& operator=(const StringList& other);  // deep copy
    ~StringList();                           // frees every node

    void pushBack(const std::string& value); // append at the tail, O(1)
    bool popFront(std::string& outValue);    // remove head; false when empty
    bool isEmpty() const;                    // true when size is zero
    int size() const;                        // number of stored strings
    void clear();                            // delete all nodes

private:
    Node* head_;   // front of the list (popFront here)
    Node* tail_;   // back of the list (keeps pushBack O(1))
    int size_;     // node count

    void copyFrom(const StringList& other);  // helper: deep-copy 'other' nodes
};
