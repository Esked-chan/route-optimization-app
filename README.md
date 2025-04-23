# Route Optimization Project

This project is a graphical application for creating, visualizing, and optimizing paths on a graph. It provides an interactive interface for adding nodes, creating connections, and calculating the shortest path between two points using Dijkstra's algorithm.

## Features

- **Graph Manipulation**:
  - Add, move, and delete nodes.
  - Create and remove connections between nodes.
  - Automatically update connection weights based on node positions.

- **Pathfinding**:
  - Calculate the shortest path between a start and end node using Dijkstra's algorithm.
  - Display travel distance, weighted distance, and estimated travel time.

- **Interactive UI**:
  - Visualize the graph and paths in a graphical interface.
  - Pan and zoom the graph view.
  - Toggle visibility of the calculated path.

- **Keyboard Shortcuts**:
  - `P`: Toggle path visibility.
  - `C`: Clear the graph.
  - Other shortcuts for switching modes (e.g., add nodes, delete nodes, etc.).

## Build Instructions

This project uses CMake for building. Follow these steps to build the project:

1. **Install Dependencies**:
   - SDL3 library (for rendering and input handling).
   - SDL3_ttf library (for text rendering).

2. **Generate Build Files**:
   ```bash
   cmake -S . -B build
   ```

3. **Build the project**:
  ```bash
  cmake --build build
  ```

4. **Run the application**: Navigate to the build directory and execute the generated binary
  ```bash
  ./route_opt_project
  ```

## Dependencies
- SDL3
- SDL3_ttf

## Disclaimer
I have not tested this on any machine other than mine. If it breaks it breaks, I'm sorry, I'm just glad it works for me.