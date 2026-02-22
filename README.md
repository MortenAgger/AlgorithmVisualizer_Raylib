# Algorithm Visualizer (Raylib)

C++-algoritme-visualizer bygget med [Raylib](https://www.raylib.com/). Visualiser **Shortest path** (grid + graf), **MST (Prim/Kruskal)** og **Heap (Min/Max)** med træ-struktur og animation.

## Krav

- **CMake** 3.16+
- **C++17**-kompilator
- **Raylib** – findes den ikke på systemet, hentes den automatisk via FetchContent

## Bygge og køre

```bash
mkdir build && cd build
cmake ..
cmake --build .
./AlgoVisualizer
```

### Tests

Projektet inkluderer unit tests (GoogleTest hentes automatisk):

```bash
cmake .. -DBUILD_TESTS=ON
cmake --build .
ctest --output-on-failure
```

## Features

### Hovedmenu

- **Shortest path** – grid-pathfinding og graf-editor med BFS, DFS, Dijkstra, A*
- **MST (Prim/Kruskal)** – minimalt spændtræ med vægtede kanter
- **Heap (Min/Max)** – binær heap med indsættelse, DeleteRoot, Heapify og array-byg
- **Luk program** – afslut

### Shortest path

- **Grid Pathfinding**: 2D-gitter, start/mål, vægge. Algoritmer: BFS, DFS, Dijkstra, A*. Presets: Empty, Random, Maze. Clear Walls / Clear Path.
- **Graph Editor**: Placér noder (klik), forbind med **Ctrl+klik** på to noder (træ-regel: hver node har højst én forælder; re-parent ved ny forbindelse). S=Start, T=Mål. Vægte ved Dijkstra/A* (Ctrl+klik på kant). Save/Load (JSON).

### MST (Prim/Kruskal)

- Venstre panel: Add Node, Select/Move, Add Edge, Delete. Klik på tomt område = ny node; Add Edge: klik to noder = kant. Dobbeltklik på kant = rediger vægt.
- Prim: vælg start-node. Kruskal: kanter sorteret i panel. Run, Step, Back, Play/Pause, Clear.

### Heap (Min/Max)

- **Insert**: Klik eller Ctrl+klik på værdiboksen for at sætte tal, derefter Insert. **DeleteRoot**: fjern rod. **Heapify**: byg heap fra nuværende array.
- **Array**: Skriv tal med komma (fx `5, 2, 9, 1, 7`), klik feltet for at redigere. **Indsæt** = indlæs array som række, derefter **Heapify** for at bygge heap.
- Min/Max-knapper skifter heap-type. Speed +/-. Esc = tilbage.

## Projektstruktur

| Mappe        | Indhold |
|-------------|---------|
| `include/`  | Headers: scener, algoritmer, datastrukturer, heap, MST, pathfinding, render, UI |
| `src/`      | Implementering: Application, MainMenu, HeapScene, PathfindingScene, MstScene, Raylib-render, algoritmer, heap, MST |
| `tests/`    | Unit tests (Graph, BFS, Heap) |

## Teknisk

- **Application**: Hovedloop (input → update → draw), scene-styring.
- **Scener**: Hver scene (Pathfinding, Mst, Heap) har ét ansvar; ESC går tilbage til menu.
- **Algoritmer**: BFS, DFS, Dijkstra, A* (IAlgorithm); Heap bruger egen engine med steps og replay.
- **Graf**: Træ-begrænsning i Graph Editor (getParentId, re-parent ved ny kant).

## Licens

Projektets kode: efter eget valg.  
Raylib: zlib/libpng.  
nlohmann/json: MIT.  
GoogleTest: BSD-3-Clause.
