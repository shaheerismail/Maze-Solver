#pragma once






#include <string>

class StringList {
private:

    struct Node {
        std::string value;
        Node* next;
    };

public:
    StringList();
    StringList(const StringList& other);
    StringList& operator=(const StringList& other);
    ~StringList();

    void pushBack(const std::string& value);
    bool popFront(std::string& outValue);
    bool isEmpty() const;
    int size() const;
    void clear();

private:
    Node* head_;
    Node* tail_;
    int size_;

    void copyFrom(const StringList& other);
};
