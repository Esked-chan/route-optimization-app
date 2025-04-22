#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <graph.h>
#include <stdbool.h>

#define MAX_PATH_LENGTH 1000

typedef struct Path {
  int nodes[MAX_PATH_LENGTH];
  int length;
  float total_cost;
  float distance;
  float time;
} Path;

bool dijkstra(const Graph *graph, int start, int end, Path* path);

#endif // PATHFINDING_H