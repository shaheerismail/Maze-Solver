// MazeLibrary implementation. Standard recursive BST algorithms; the only
// change from the approved version is the standalone MazeRecordList output.

#include "maze_library.h"

// An empty library has no root and holds zero mazes.
MazeLibrary::MazeLibrary() : root_(0), size_(0) {}

// Destructor: frees the whole tree post-order.
MazeLibrary::~MazeLibrary()
{
    destroy(root_);
    root_ = 0;
    size_ = 0;
}

// Post-order free: children first, then the node itself.
void MazeLibrary::destroy(Node* node)
{
    if (node == 0)
        return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

// Public insert: replaces the record when the seed already exists.
void MazeLibrary::insert(const MazeRecord& record)
{
    root_ = insert(root_, record);
}

// Recursive BST insert: descend to a leaf slot, or overwrite on equal seed.
MazeLibrary::Node* MazeLibrary::insert(Node* node, const MazeRecord& record)
{
    if (node == 0) {                       // fell off the tree: attach new leaf
        Node* fresh = new Node;
        fresh->record = record;
        fresh->left = 0;
        fresh->right = 0;
        ++size_;
        return fresh;
    }
    if (record.seed < node->record.seed) {
        node->left = insert(node->left, record);
    } else if (record.seed > node->record.seed) {
        node->right = insert(node->right, record);
    } else {
        node->record = record;             // duplicate seed: replace in place
    }
    return node;
}

// Public lookup: fills 'out' with the record; false when the seed is absent.
bool MazeLibrary::findBySeed(unsigned int seed, MazeRecord& out) const
{
    Node* hit = find(root_, seed);
    if (hit == 0)
        return false;
    out = hit->record;
    return true;
}

// Recursive BST lookup by seed: go left (<), right (>), or stop (=).
MazeLibrary::Node* MazeLibrary::find(Node* node, unsigned int seed) const
{
    if (node == 0)
        return 0;
    if (seed == node->record.seed)
        return node;
    if (seed < node->record.seed)
        return find(node->left, seed);
    return find(node->right, seed);
}

// Leftmost node of a subtree: the in-order successor candidate for deletes.
MazeLibrary::Node* MazeLibrary::minValueNode(Node* node) const
{
    Node* cur = node;
    while (cur != 0 && cur->left != 0)
        cur = cur->left;
    return cur;
}

// Public remove: true when a node with this seed was deleted.
bool MazeLibrary::removeBySeed(unsigned int seed)
{
    bool removed = false;
    root_ = remove(root_, seed, removed);
    return removed;
}

// Recursive 3-case BST delete: leaf/one-child splice, or successor swap.
MazeLibrary::Node* MazeLibrary::remove(Node* node, unsigned int seed, bool& removed)
{
    if (node == 0)
        return 0;
    if (seed < node->record.seed) {
        node->left = remove(node->left, seed, removed);
    } else if (seed > node->record.seed) {
        node->right = remove(node->right, seed, removed);
    } else {
        removed = true;
        if (node->left == 0) {             // leaf or right-child-only case: splice
            Node* keep = node->right;
            delete node;
            --size_;
            return keep;
        }
        if (node->right == 0) {            // left-child-only case
            Node* keep = node->left;
            delete node;
            --size_;
            return keep;
        }
        // two children: copy the in-order successor's record into this node,
        // then delete the successor node from the right subtree (that
        // recursion performs the actual size_ decrement)
        Node* successor = minValueNode(node->right);
        node->record = successor->record;
        bool justSpliced = false;
        node->right = remove(node->right, successor->record.seed, justSpliced);
    }
    return node;
}

// Number of stored mazes.
int MazeLibrary::size() const { return size_; }

// Public in-order traversal: appends every record sorted by seed.
void MazeLibrary::inOrder(MazeRecordList& out) const
{
    inOrder(root_, out);
}

// Recursive in-order walk: left subtree, this record, right subtree.
void MazeLibrary::inOrder(Node* node, MazeRecordList& out) const
{
    if (node == 0)
        return;
    inOrder(node->left, out);
    out.pushBack(node->record);
    inOrder(node->right, out);
}
