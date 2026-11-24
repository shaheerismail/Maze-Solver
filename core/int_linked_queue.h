#pragma once






class IntLinkedQueue {
private:

    struct Node {
        int value;
        Node* next;
    };

public:
    IntLinkedQueue();
    IntLinkedQueue(const IntLinkedQueue& other);
    IntLinkedQueue& operator=(const IntLinkedQueue& other);
    ~IntLinkedQueue();

    void enqueue(int value);
    bool dequeue(int& outValue);
    bool isEmpty() const;
    int size() const;
    void clear();

private:
    Node* head_;
    Node* tail_;
    int size_;

    void copyFrom(const IntLinkedQueue& other);
};
