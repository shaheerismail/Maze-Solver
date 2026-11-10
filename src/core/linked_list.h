#pragma once
// LinkedList<T> — the project's own singly linked list, used for adjacency
// lists, recorded animation steps and BFS routes. Header-only: it is a
// template, so the definitions must be visible to every user.
//
// Ownership: the list owns every node; nodes are never shared or exposed.
// clear() and the destructor delete in order, so there is no leak.

template <typename T>
class LinkedList {
private:
    struct Node {
        T value;
        Node* next;
    };

public:
    LinkedList() : head_(0), tail_(0), size_(0) {}

    LinkedList(const LinkedList& other) : head_(0), tail_(0), size_(0)
    {
        copyFrom(other);
    }

    LinkedList& operator=(const LinkedList& other)
    {
        if (this != &other) {
            clear();
            copyFrom(other);
        }
        return *this;
    }

    ~LinkedList() { clear(); }

    void pushBack(const T& value)  // O(1) via tail_
    {
        Node* node = new Node;
        node->value = value;
        node->next = 0;
        if (tail_ != 0) {
            tail_->next = node;
        } else {
            head_ = node;
        }
        tail_ = node;
        ++size_;
    }

    void pushFront(const T& value)  // O(1): BFS route reconstruction
    {
        Node* node = new Node;
        node->value = value;
        node->next = head_;
        head_ = node;
        if (tail_ == 0)
            tail_ = node;
        ++size_;
    }

    bool popFront(T& out)
    {
        if (head_ == 0)
            return false;
        out = head_->value;
        Node* dead = head_;
        head_ = head_->next;
        if (head_ == 0)
            tail_ = 0;
        delete dead;
        --size_;
        return true;
    }

    bool isEmpty() const { return size_ == 0; }
    int size() const { return size_; }

    void clear()
    {
        while (head_ != 0) {
            Node* dead = head_;
            head_ = head_->next;
            delete dead;
        }
        tail_ = 0;
        size_ = 0;
    }

    // Java-style forward iterator over a const list: next() returns the
    // current element and advances. Used for drawing and for BST in-order dumps.
    class Iterator {
    public:
        bool hasNext() const { return node_ != 0; }
        const T& next()
        {
            const T& value = node_->value;
            node_ = node_->next;
            return value;
        }

    private:
        friend class LinkedList<T>;
        explicit Iterator(Node* node) : node_(node) {}
        Node* node_;
    };

    Iterator iterator() const { return Iterator(head_); }

private:
    Node* head_;
    Node* tail_;
    int size_;

    void copyFrom(const LinkedList& other)
    {
        for (Node* n = other.head_; n != 0; n = n->next)
            pushBack(n->value);
    }
};
