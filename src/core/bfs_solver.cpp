// BfsSolver implementation. The search itself is unchanged textbook BFS:
// seen[] flags, parent[] chain, an IntLinkedQueue frontier — the approved
// logic; only the containers are now standalone DSA classes.

#include "bfs_solver.h"

// BFS from 'source' to 'target'; fills outRoute (source -> target) and the
// animation step list. Returns true only when the target is reached.
bool BfsSolver::solve(const MazeGraph& graph, int source, int target, IntLinkedList& outRoute,
                      const IntLinkedList* blockedVertices)
{
    steps_.clear();
    outRoute.clear();

    int n = graph.vertexCount();
    if (source < 0 || source >= n || target < 0 || target >= n)
        return false;

    // per-call scratch: seen flags + parent chain, freed before EVERY return
    unsigned char* seen = new unsigned char[n];
    int* parent = new int[n];
    for (int i = 0; i < n; ++i) {
        seen[i] = 0;
        parent[i] = -1;
    }

    IntLinkedQueue frontier;
    seen[source] = 1;
    frontier.enqueue(source);

    bool found = (source == target);   // 1x1 maze: entrance == exit
    while (!found && !frontier.isEmpty()) {
        int v;
        frontier.dequeue(v);

        SolveStep visit;
        visit.kind = SolveStep::Dequeued;
        visit.vertex = v;
        visit.neighbor = -1;
        visit.discovered = false;
        steps_.pushBack(visit);

        if (v == target) {
            found = true;
            break;
        }
        IntLinkedList::Cursor it = graph.neighbors(v).cursor();
        while (it.hasNext()) {
            int w = it.next();
            bool blocked = (blockedVertices != 0 && blockedVertices->contains(w));
            bool discovered = (seen[w] == 0) && !blocked;

            SolveStep check;
            check.kind = SolveStep::EdgeChecked;
            check.vertex = v;
            check.neighbor = w;
            check.discovered = discovered;
            steps_.pushBack(check);

            if (discovered) {
                seen[w] = 1;
                parent[w] = v;
                frontier.enqueue(w);
            }
        }
    }

    if (found) {
        // walk the discovery chain from exit back to entrance; pushFront
        // gives source -> target order directly, no reversal pass needed
        for (int at = target; at != -1; at = parent[at])
            outRoute.pushFront(at);

        SolveStep done;
        done.kind = SolveStep::Found;
        done.vertex = target;
        done.neighbor = -1;
        done.discovered = false;
        steps_.pushBack(done);
    }

    delete[] seen;   // single exit point: arrays freed only after every use
    delete[] parent;
    return found;
}

// Animation record of the most recent solve() call.
const SolveStepList& BfsSolver::steps() const
{
    return steps_;
}
