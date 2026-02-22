# Algorithm Visualizer (Raylib)

A C++ algorithm visualizer built with [Raylib](https://www.raylib.com/). Visualize **Shortest Path** (grid + graph), **MST (Prim/Kruskal)**, and **Heap (Min/Max)** with tree-style rendering and animations.

## Requirements

- **CMake** 3.16+
- **C++17** compiler
- **Raylib** – if not found on the system, it is fetched automatically via FetchContent

## Build and run

```bash
mkdir build && cd build
cmake ..
cmake --build .
./AlgoVisualizer
```

## Tests

The project includes unit tests (GoogleTest is fetched automatically):

```bash
cmake .. -DBUILD_TESTS=ON
cmake --build .
ctest --output-on-failure
```

## Features

### Main Menu

- **Shortest Path** – grid pathfinding and a graph editor with BFS, DFS, Dijkstra, A*
- **MST (Prim/Kruskal)** – minimum spanning tree with weighted edges
- **Heap (Min/Max)** – binary heap with insert, DeleteRoot, Heapify, and build-from-array
- **Exit** – quit the program

## Shortest Path

### Grid Pathfinding

- 2D grid with start/goal and walls
- Algorithms: BFS, DFS, Dijkstra, A*
- Presets: Empty, Random, Maze
- Actions: Clear Walls / Clear Path

### Graph Editor

- Place nodes (click)
- Connect nodes using **Ctrl+click** on two nodes  
  (tree rule: each node has at most one parent; re-parent on new connection)
- **S** = Start, **T** = Target
- Weighted edges for Dijkstra/A* (**Ctrl+click** on an edge to edit weight)
- Save/Load graphs (JSON)

## MST (Prim/Kruskal)

- Tools: Add Node, Select/Move, Add Edge, Delete
- Click empty space = new node
- Add Edge: click two nodes = edge
- Double-click an edge = edit weight
- Prim: select a start node
- Kruskal: edges are shown sorted in a panel
- Controls: Run, Step, Back, Play/Pause, Clear

## Heap (Min/Max)

- Insert: click (or **Ctrl+click**) the value box to enter a number, then press **Insert**
- DeleteRoot: remove the root node
- Heapify: build a heap from the current array

### Array input

- Enter comma-separated values (e.g. `5, 2, 9, 1, 7`)
- Press **Insert** to load the array row, then **Heapify** to build the heap

### Controls

- Min/Max buttons switch heap type
- Speed controls: Speed + / -
- **Esc** = back to menu

## Project Structure

| Folder     | Contents |
|------------|----------|
| `include/` | Headers: scenes, algorithms, data structures, heap, MST, pathfinding, rendering, UI |
| `src/`     | Implementation: Application, MainMenu, HeapScene, PathfindingScene, MstScene, Raylib rendering, algorithms, heap, MST |
| `tests/`   | Unit tests (Graph, BFS, Heap) |

## Technical Notes

- **Application**: main loop (input → update → draw) and scene management.
- **Scenes**: each scene (Pathfinding, MST, Heap) has a single responsibility; **Esc** returns to the menu.
- **Algorithms**: BFS, DFS, Dijkstra, A* via an `IAlgorithm` interface. Heap uses its own step/replay engine.
- **Graph**: tree constraint in the Graph Editor (`getParentId`, re-parent on new edge).

## License

- Project code: your choice.
- Raylib: zlib/libpng.
- nlohmann/json: MIT.
- GoogleTest: BSD-3-Clause.