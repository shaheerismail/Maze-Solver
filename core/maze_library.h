#pragma once







#include "steps.h"
#include "maze_record_list.h"


class MazeLibrary {
public:
    MazeLibrary();
    MazeLibrary(const MazeLibrary&) = delete;
    MazeLibrary& operator=(const MazeLibrary&) = delete;
    ~MazeLibrary();

    void insert(const MazeRecord& record);
    bool findBySeed(unsigned int seed, MazeRecord& out) const;
    bool removeBySeed(unsigned int seed);
    int size() const;
    void inOrder(MazeRecordList& out) const;

private:

    struct Node {
        MazeRecord record;
        Node* left;
        Node* right;
    };

    Node* root_;
    int size_;

    Node* insert(Node* node, const MazeRecord& record);
    Node* find(Node* node, unsigned int seed) const;
    Node* remove(Node* node, unsigned int seed, bool& removed);
    Node* minValueNode(Node* node) const;
    void inOrder(Node* node, MazeRecordList& out) const;
    void destroy(Node* node);
};
