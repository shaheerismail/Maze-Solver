#pragma once





class IntLinkedList {
private:

    struct Node {
        int value;
        Node* next;
    };

public:


    class Cursor {
    public:
        Cursor();
        bool hasNext() const;
        int next();
    private:
        friend class IntLinkedList;
        explicit Cursor(const Node* start);
        const Node* node_;
    };

    IntLinkedList();
    IntLinkedList(const IntLinkedList& other);
    IntLinkedList& operator=(const IntLinkedList& other);
    ~IntLinkedList();

    void pushBack(int value);
    void pushFront(int value);
    bool popFront(int& outValue);
    bool isEmpty() const;
    int size() const;
    bool contains(int value) const;
    bool removeValue(int value);
    void clear();
    Cursor cursor() const;

private:
    Node* head_;
    Node* tail_;
    int size_;

    void copyFrom(const IntLinkedList& other);
};
