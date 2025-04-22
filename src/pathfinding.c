#include "pathfinding.h"
#include <float.h>
#include <stdlib.h>

typedef struct {
  int id;
  float distance;
  int previous;
} NodePriority;

static int compare_nodes(const void* a, const void* b) {
  const NodePriority* nodeA = (const NodePriority*)a;
  const NodePriority* nodeB = (const NodePriority*)b;
  if (nodeA->distance < nodeB->distance) return -1;
  if (nodeA->distance > nodeB->distance) return 1;
  return 0;
}

bool dijkstra(const Graph* graph, int start, int end, Path* path) {
  if (start < 0 || start >= graph->node_count || end < 0 || end >= graph->node_count) {
    SDL_Log("Invalid start or end node index: %d, %d", start, end);
    return false;
  }

  if (start == end) {
    path->nodes[0] = start;
    path->length = 1;
    path->total_cost = 0.0f;
    path->distance = 0.0f;
    path->time = 0.0f;
    return true;
  }

  NodePriority priorities[MAX_NODES];
  bool visited[MAX_NODES] = {false};

  for (int i = 0; i < graph->node_count; i++) {
    priorities[i].id = i;
    priorities[i].distance = FLT_MAX;
    priorities[i].previous = -1;
  }

  priorities[start].distance = 0;
  path->distance = 0.f;

  for (int unvisited_count = graph->node_count; unvisited_count > 0; unvisited_count--) {
    int current = -1;
    float min_distance = FLT_MAX;

    for (int i = 0; i < graph->node_count; i++) {
      if (!visited[priorities[i].id] && priorities[i].distance < min_distance) {
        current = priorities[i].id;
        min_distance = priorities[i].distance;
      } 
    }

    if (current == -1 || min_distance == FLT_MAX) {
      break;
    }

    visited[current] = true;

    if (current == end) {
      break;
    }

    const Node* node = &graph->nodes[current];
    for (int i = 0; i < node->connection_count; i++) {
      int neighbor = node->connections[i];
      if (visited[neighbor]) {
        continue;
      }

      float new_distance = priorities[current].distance + node->connection_weights[i];

      for (int j = 0; j < graph->node_count; j++) {
        if (priorities[j].id == neighbor) {
          if (new_distance < priorities[j].distance) {
            priorities[j].distance = new_distance;
            priorities[j].previous = current;
          }
          break;
        }
      }
    }
  }

  if (priorities[end].previous == -1 && start != end) {
    SDL_Log("No path found from %d to %d", start, end);
    return false;
  }

  path->length = 0;
  int current = end;
  while (current != -1 && path->length < MAX_PATH_LENGTH) {
    path->nodes[path->length++] = current;
    for (int i = 0; i < graph->node_count; i++) {
      if (priorities[i].id == current) {
        current = priorities[i].previous;
        break;
      }
    }
  }

  for (int i = 0; i < path->length / 2; i++) {
    int temp = path->nodes[i];
    path->nodes[i] = path->nodes[path->length - 1 - i];
    path->nodes[path->length - 1 - i] = temp;
  }

  path->distance = 0.f;
  for (int i = 0; i < path->length - 1; i++) {
    int from = path->nodes[i];
    int to = path->nodes[i + 1];
    for (int j = 0; j < graph->nodes[from].connection_count; j++) {
      if (graph->nodes[from].connections[j] == to) {
        path->distance += graph->nodes[from].connection_lengths[j];
        break;
      }
    }
  }

  float speed = (50.f / 60.f) * 1000; // ~833.3 m/min (50 km/h)

  path->total_cost = priorities[end].distance;
  float travel_time = path->total_cost / speed;
  path->time = travel_time;
  SDL_Log("Path found from %d to %d, total distance is %.1f metres, travel time is %.2f minutes.", start, end, path->distance, travel_time);
  return true;
}