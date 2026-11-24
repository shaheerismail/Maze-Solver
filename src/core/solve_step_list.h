#pragma once





#include "steps.h"

class SolveStepList {
private:

    struct Node {
        SolveStep step;
        Node* next;
    };

public:


    class Cursor {
    public:
        Cursor();
        bool hasNext() const;
        const SolveStep& next();
    private:
        friend class SolveStepList;
        explicit Cursor(const Node* start);
        const Node* node_;
    };

    SolveStepList();
    SolveStepList(const SolveStepList& other);
    SolveStepList& operator=(const SolveStepList& other);
    ~SolveStepList();

    void pushBack(const SolveStep& step);
    bool isEmpty() const;
    int size() const;
    void clear();
    Cursor cursor() const;

private:
    Node* head_;
    Node* tail_;
    int size_;

    void copyFrom(const SolveStepList& other);
};
