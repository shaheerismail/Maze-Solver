


#include "maze_library.h"


MazeLibrary::MazeLibrary() : root_(0), size_(0) {}


MazeLibrary::~MazeLibrary()
{
    destroy(root_);
    root_ = 0;
    size_ = 0;
}


void MazeLibrary::destroy(Node* node)
{
    if (node == 0)
        return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}


void MazeLibrary::insert(const MazeRecord& record)
{
    root_ = insert(root_, record);
}


MazeLibrary::Node* MazeLibrary::insert(Node* node, const MazeRecord& record)
{
    if (node == 0) {
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
        node->record = record;
    }
    return node;
}


bool MazeLibrary::findBySeed(unsigned int seed, MazeRecord& out) const
{
    Node* hit = find(root_, seed);
    if (hit == 0)
        return false;
    out = hit->record;
    return true;
}


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


MazeLibrary::Node* MazeLibrary::minValueNode(Node* node) const
{
    Node* cur = node;
    while (cur != 0 && cur->left != 0)
        cur = cur->left;
    return cur;
}


bool MazeLibrary::removeBySeed(unsigned int seed)
{
    bool removed = false;
    root_ = remove(root_, seed, removed);
    return removed;
}


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
        if (node->left == 0) {
            Node* keep = node->right;
            delete node;
            --size_;
            return keep;
        }
        if (node->right == 0) {
            Node* keep = node->left;
            delete node;
            --size_;
            return keep;
        }



        Node* successor = minValueNode(node->right);
        node->record = successor->record;
        bool justSpliced = false;
        node->right = remove(node->right, successor->record.seed, justSpliced);
    }
    return node;
}


int MazeLibrary::size() const { return size_; }


void MazeLibrary::inOrder(MazeRecordList& out) const
{
    inOrder(root_, out);
}


void MazeLibrary::inOrder(Node* node, MazeRecordList& out) const
{
    if (node == 0)
        return;
    inOrder(node->left, out);
    out.pushBack(node->record);
    inOrder(node->right, out);
}
