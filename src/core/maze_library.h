#pragma once
// MazeLibrary — binary search tree of saved mazes, keyed by seed.
// Search/insert/remove walk the tree recursively; in-order traversal gives
// the records sorted by seed. Re-saving an existing seed replaces its record.
//
// Ownership: the tree owns every Node (and the MazeRecord inside, including
// its walls string). destroy() frees post-order; nothing is shared outside.

#include "steps.h"
#include "linked_list.h"

class MazeLibrary {
public:
    MazeLibrary();
    MazeLibrary(const MazeLibrary&) = delete;            // owning tree, not copyable
    MazeLibrary& operator=(const MazeLibrary&) = delete;
    ~MazeLibrary();

    void insert(const MazeRecord& record);                // replaces on duplicate seed
    bool findBySeed(unsigned int seed, MazeRecord& out) const;
    bool removeBySeed(unsigned int seed);                 // classic 3-case BST delete
    int size() const;
    void inOrder(LinkedList<MazeRecord>& out) const;      // ascending by seed

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
    Node* minValueNode(Node* node) const;                 // in-order successor helper
    void inOrder(Node* node, LinkedList<MazeRecord>& out) const;
    void destroy(Node* node);
};
