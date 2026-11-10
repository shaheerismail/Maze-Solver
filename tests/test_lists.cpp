#include "test_util.h"
#include "linked_list.h"
#include "queue.h"

void testLists()
{
    // T06: pushBack then popFront preserves order
    LinkedList<int> list;
    list.pushBack(1);
    list.pushBack(2);
    list.pushBack(3);
    CHECK_EQ(list.size(), 3);
    int value = 0;
    CHECK(list.popFront(value));
    CHECK_EQ(value, 1);
    CHECK(list.popFront(value));
    CHECK_EQ(value, 2);
    CHECK(list.popFront(value));
    CHECK_EQ(value, 3);
    CHECK(!list.popFront(value));  // 4th pop: empty -> false, no crash

    // T07: pushFront — head-first insertion (used by BFS route building)
    LinkedList<int> front;
    front.pushFront(1);
    front.pushFront(2);
    front.pushFront(3);
    front.popFront(value);  // newest first
    CHECK_EQ(value, 3);
    front.popFront(value);
    CHECK_EQ(value, 2);
    CHECK_EQ(front.size(), 1);

    // T08: deep copy independence
    LinkedList<int> orig;
    orig.pushBack(10);
    orig.pushBack(20);
    LinkedList<int> copyOf(orig);   // copy constructor
    copyOf.pushBack(30);
    CHECK_EQ(orig.size(), 2);       // original intact
    CHECK_EQ(copyOf.size(), 3);
    LinkedList<int> assigned;
    assigned.pushBack(99);
    assigned = orig;                // copy assignment clears old nodes
    assigned.pushBack(40);
    CHECK_EQ(orig.size(), 2);       // assignment never shares nodes
    CHECK_EQ(assigned.size(), 3);
    orig = orig;                    // self-assignment is safe
    CHECK_EQ(orig.size(), 2);

    // T09: FIFO order under interleaving (generate, solve, enqueue again)
    LinkedQueue<int> queue;
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    CHECK(!queue.isEmpty());
    queue.dequeue(value);           // dequeue; enqueue; dequeue; ...
    CHECK_EQ(value, 1);
    queue.enqueue(4);
    queue.dequeue(value);
    CHECK_EQ(value, 2);
    queue.dequeue(value);
    CHECK_EQ(value, 3);
    queue.dequeue(value);
    CHECK_EQ(value, 4);
    CHECK(!queue.dequeue(value));   // queue drained safely
    CHECK_EQ(queue.size(), 0);
    LinkedQueue<int> qc(queue);     // copy of an empty queue
    qc.enqueue(5);
    LinkedQueue<int> qc2(qc);
    CHECK_EQ(qc2.size(), 1);
    qc2.dequeue(value);
    CHECK_EQ(value, 5);
}
