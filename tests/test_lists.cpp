#include "test_util.h"
#include "int_linked_list.h"
#include "int_linked_queue.h"
#include "carve_step_list.h"
#include "solve_step_list.h"
#include "maze_record_list.h"
#include "string_list.h"
#include "steps.h"
#include "maze_matrix.h"

// Suite T06–T09b: the standalone DSA containers (no templates in the project).
void testLists()
{
    // T06: pushBack then popFront preserves order
    IntLinkedList list;
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
    IntLinkedList front;
    front.pushFront(1);
    front.pushFront(2);
    front.pushFront(3);
    front.popFront(value);  // newest first
    CHECK_EQ(value, 3);
    front.popFront(value);
    CHECK_EQ(value, 2);
    CHECK_EQ(front.size(), 1);

    // T07b: contains and removeValue on the standalone int list
    IntLinkedList members;
    members.pushBack(5);
    members.pushBack(7);
    members.pushBack(9);
    CHECK(members.contains(7));
    CHECK(!members.contains(8));
    CHECK(members.removeValue(7));     // middle node unlinked
    CHECK_EQ(members.size(), 2);
    CHECK(!members.contains(7));
    CHECK(!members.removeValue(7));    // already gone
    members.removeValue(5);
    members.removeValue(9);
    CHECK(members.isEmpty());
    members.clear();
    CHECK_EQ(members.size(), 0);

    // T08: deep copy independence
    IntLinkedList orig;
    orig.pushBack(10);
    orig.pushBack(20);
    IntLinkedList copyOf(orig);   // copy constructor
    copyOf.pushBack(30);
    CHECK_EQ(orig.size(), 2);       // original intact
    CHECK_EQ(copyOf.size(), 3);
    IntLinkedList assigned;
    assigned.pushBack(99);
    assigned = orig;                // copy assignment clears old nodes
    assigned.pushBack(40);
    CHECK_EQ(orig.size(), 2);       // assignment never shares nodes
    CHECK_EQ(assigned.size(), 3);
    orig = orig;                    // self-assignment is safe
    CHECK_EQ(orig.size(), 2);

    // T09: FIFO order under interleaving (generate, solve, enqueue again)
    IntLinkedQueue queue;
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
    IntLinkedQueue qc(queue);       // copy of an empty queue
    qc.enqueue(5);
    IntLinkedQueue qc2(qc);
    CHECK_EQ(qc2.size(), 1);
    qc2.dequeue(value);
    CHECK_EQ(value, 5);

    // T09b: step/record/string lists append, walk and copy correctly
    CarveStep carveStep;
    carveStep.kind = CarveStep::Carve;
    carveStep.row = 1;
    carveStep.col = 2;
    carveStep.side = MazeMatrix::WALL_E;
    CarveStepList genSteps;
    genSteps.pushBack(carveStep);
    CHECK_EQ(genSteps.size(), 1);
    CarveStepList genCopy(genSteps);   // deep copy of the step list
    genSteps.clear();
    CHECK_EQ(genSteps.size(), 0);
    CHECK_EQ(genCopy.size(), 1);
    CHECK_EQ(genCopy.cursor().next().col, 2);

    SolveStep solveStep;
    solveStep.kind = SolveStep::EdgeChecked;
    solveStep.vertex = 4;
    solveStep.neighbor = 5;
    solveStep.discovered = true;
    SolveStepList solveSteps;
    solveSteps.pushBack(solveStep);
    CHECK_EQ(solveSteps.size(), 1);
    CHECK(solveSteps.cursor().next().discovered);

    MazeRecord record;
    record.seed = 77u;
    record.rows = 2;
    record.cols = 2;
    record.extraPassages = 0;
    record.walls = "ABCD";
    MazeRecordList records;
    records.pushBack(record);
    CHECK_EQ(records.size(), 1);
    CHECK_EQ(records.cursor().next().seed, 77u);

    StringList lines;
    lines.pushBack("MAZE_V1");
    lines.pushBack("rows 2");
    std::string text;
    CHECK(lines.popFront(text));
    CHECK_EQ(text, std::string("MAZE_V1"));
    CHECK(lines.popFront(text));
    CHECK_EQ(text, std::string("rows 2"));
    CHECK(!lines.popFront(text));   // drained safely
    CHECK_EQ(lines.size(), 0);
}
