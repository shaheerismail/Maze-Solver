#pragma once
// MazeLibrary — binary search tree of saved mazes, keyed by seed.
// Search/insert/remove walk the tree recursively; in-order traversal gives
// the records sorted by seed. Re-saving an existing seed replaces its record.
//
// Ownership: the tree owns every Node (and the MazeRecord inside, including
// its walls string). destroy() frees post-order; nothing is shared outside.

#include "steps.h"
#include "maze_record_list.h"

// Binary search tree storing one MazeRecord per seed.
class MazeLibrary {
public:
    MazeLibrary();                              // empty tree
    MazeLibrary(const MazeLibrary&) = delete;   // owning tree, not copyable
    MazeLibrary& operator=(const MazeLibrary&) = delete;
    ~MazeLibrary();                             // frees the whole tree

    void insert(const MazeRecord& record);      // replaces on duplicate seed
    bool findBySeed(unsigned int seed, MazeRecord& out) const; // false when absent
    bool removeBySeed(unsigned int seed);       // classic 3-case BST delete
    int size() const;                           // number of stored mazes
    void inOrder(MazeRecordList& out) const;    // ascending by seed

private:
    // One tree node holding a saved maze and its two subtrees.
    struct Node {
        MazeRecord record;
        Node* left;
        Node* right;
    };

    Node* root_;   // tree root, 0 when the library is empty
    int size_;     // number of stored mazes

    Node* insert(Node* node, const MazeRecord& record);   // recursive insert/replace
    Node* find(Node* node, unsigned int seed) const;      // recursive lookup
    Node* remove(Node* node, unsigned int seed, bool& removed); // recursive delete
    Node* minValueNode(Node* node) const;                 // in-order successor helper
    void inOrder(Node* node, MazeRecordList& out) const;  // recursive traversal
    void destroy(Node* node);                             // post-order node freeing
};
