#pragma once






#include "steps.h"

class MazeRecordList {
private:

    struct Node {
        MazeRecord record;
        Node* next;
    };

public:


    class Cursor {
    public:
        Cursor();
        bool hasNext() const;
        const MazeRecord& next();
    private:
        friend class MazeRecordList;
        explicit Cursor(const Node* start);
        const Node* node_;
    };

    MazeRecordList();
    MazeRecordList(const MazeRecordList& other);
    MazeRecordList& operator=(const MazeRecordList& other);
    ~MazeRecordList();

    void pushBack(const MazeRecord& record);
    bool isEmpty() const;
    int size() const;
    void clear();
    Cursor cursor() const;

private:
    Node* head_;
    Node* tail_;
    int size_;

    void copyFrom(const MazeRecordList& other);
};
