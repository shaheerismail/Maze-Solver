#pragma once





#include "steps.h"

class CarveStepList {
private:

    struct Node {
        CarveStep step;
        Node* next;
    };

public:


    class Cursor {
    public:
        Cursor();
        bool hasNext() const;
        const CarveStep& next();
    private:
        friend class CarveStepList;
        explicit Cursor(const Node* start);
        const Node* node_;
    };

    CarveStepList();
    CarveStepList(const CarveStepList& other);
    CarveStepList& operator=(const CarveStepList& other);
    ~CarveStepList();

    void pushBack(const CarveStep& step);
    bool isEmpty() const;
    int size() const;
    void clear();
    Cursor cursor() const;

private:
    Node* head_;
    Node* tail_;
    int size_;

    void copyFrom(const CarveStepList& other);
};
