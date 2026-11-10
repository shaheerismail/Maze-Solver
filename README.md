# Maze Solver

**Data Structures Project**
National University of Computer & Emerging Sciences, Karachi Campus
Section: BCS-3E

**Author:** 25K-0676 Muhammad Shaheer Ismail Khan

---

## Introduction

Maze Solver is a Qt desktop application, written in C++, that generates a maze and finds the shortest route through it. The maze is stored in a 2D array matrix and built with a recursive backtracking algorithm. It is then converted into a graph, and a breadth-first search (BFS) on that graph finds the optimal route from the entrance to the exit. The maze, the search and the final route are shown in a graphical window.

Every data structure (matrix, graph, queue, linked list and binary search tree) is implemented from scratch, without the language's built-in containers. The aim is to show how these structures work together in one complete program.

---

## Existing System

Maze generators and solvers are common as terminal programs, web visualisers and teaching demos. The algorithms themselves are well documented, for example in the Wikipedia articles on maze generation and maze-solving algorithms and in Jamis Buck's book **Mazes for Programmers**.

Many simple implementations share the same limits:

- They use one generation algorithm and one solving algorithm, so there is nothing to compare.
- They rely on built-in containers such as `std::stack` and `std::queue`, which hides the data structure that makes the algorithm work.
- They produce only perfect mazes (exactly one path between any two cells), so a search for the shortest route has nothing to optimise.
- They run in the console only, so it is hard to see how the algorithm moves through the maze.

---

## Problem Statement

Existing demos show that an algorithm works but not how the underlying data structures are built or why they fit. Many also generate only mazes with a single possible route, so the difference between any path and the optimal path is never seen.

This system builds all of its data structures itself, represents the maze both as a 2D matrix and as a graph, can add extra openings so that several routes exist and BFS must find the shortest one, and shows everything live in a Qt window.

---

## Proposed Solution

The program is divided into the parts below. Each part has a clear job and a small set of function names, so the parts can be built and tested one at a time.

| **Part** | **Data Structure or Algorithm and Its Role** |
| --- | --- |
| **Maze matrix** | 2D array of cells storing walls and open passages; used for generation, drawing and saving. |
| **Generation** | Recursion and backtracking: carve passages from a start cell, and step back when no unvisited neighbour is left. |
| **Optimal route** | Custom queue and BFS on the graph; the route is rebuilt from parent links into a linked list. |
| **Qt GUI** | Qt Widgets window that draws the matrix, animates generation and solving, and holds the controls. |

Qt is used only for the window, drawing, timers and file dialogs. Qt's own containers (such as `QVector` or `QQueue`) are not used for any of the project's data structures, and the data structure code is kept separate from the GUI so it can be tested without Qt.

---

## Salient Features

- **Maze generation:** recursive backtracking with adjustable size and a seed, so the same maze can be recreated.
- **Optimal route:** BFS on the maze graph always finds the shortest route from entrance to exit.
- **Qt interface:** maze drawn in a window, with animated generation.
- **Saved maze library:** binary search tree of saved mazes, searchable by seed.
- **Save and load:** mazes stored as text files, with checks for invalid sizes and malformed files.
- **Safe limits:** a maximum maze size to keep recursion depth under control.
- **Tests:** normal and edge cases including a 1x1 maze, a single row or column, invalid input, a maze with no route and a large maze.

---

## Tools and Technologies

| **Category** | **Details** |
| --- | --- |
| **Language** | C++ |
| **Framework** | Qt 6 (Widgets) for the GUI, built with Qt Creator or CMake |
| **Operating System** | Windows |
| **Version Control** | Git repository |
| **IDE** | VS Code |
