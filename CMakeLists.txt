cmake_minimum_required(VERSION 3.16)
project(AlgoVisualizer VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

# Raylib: find package or fetch
find_package(raylib QUIET)
if(NOT raylib_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 4.5.0
  )
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(raylib)
endif()

# nlohmann/json: use bundled third_party if present (from json release tarball), else FetchContent
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/include/nlohmann/json.hpp")
  add_library(nlohmann_json INTERFACE)
  target_include_directories(nlohmann_json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/include)
  add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
else()
  include(FetchContent)
  FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  )
  FetchContent_MakeAvailable(json)
endif()

# Core library (algorithms, datastructures, serialization, pathfinding) - no Raylib
add_library(AlgoVisualizerCore STATIC
  src/algorithms/BFS.cpp
  src/algorithms/DFS.cpp
  src/algorithms/Dijkstra.cpp
  src/algorithms/AStar.cpp
  src/datastructures/Graph.cpp
  src/datastructures/Heap.cpp
  src/serialization/StateSerializer.cpp
  src/pathfinding/EventPlayer.cpp
  src/pathfinding/GraphEventRecorder.cpp
  src/pathfinding/GridGraph.cpp
  src/pathfinding/GraphAdapter.cpp
  src/heap/HeapModel.cpp
  src/heap/HeapEngine.cpp
  src/heap/HeapStepPlayer.cpp
  src/heap/HeapController.cpp
  src/mst/Graph.cpp
  src/mst/HitTest.cpp
  src/mst/UnionFind.cpp
  src/mst/PrimMst.cpp
  src/mst/KruskalMst.cpp
  src/mst/SimulationEngine.cpp
)
target_include_directories(AlgoVisualizerCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(AlgoVisualizerCore PUBLIC nlohmann_json::nlohmann_json)

# Main executable
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/Application.cpp
  src/MainMenuScene.cpp
  src/HeapScene.cpp
  src/PathfindingScene.cpp
  src/MstScene.cpp
  src/render/RaylibRenderer.cpp
  src/ui/Button.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib AlgoVisualizerCore)

# Optional: GoogleTest for tests
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  add_executable(AlgoVisualizerTests
    tests/GraphTest.cpp
    tests/BFSTest.cpp
    tests/HeapTest.cpp
  )
  target_include_directories(AlgoVisualizerTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(AlgoVisualizerTests PRIVATE GTest::gtest_main AlgoVisualizerCore)
  include(GoogleTest)
  gtest_discover_tests(AlgoVisualizerTests)
endif()
