#include "import.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool load_nodes_from_file(Graph* graph, const char* filename) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    return false;
  }

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    int id;
    double x, y;
    if (sscanf(line, "%d,%lf,%lf", &id, &x, &y) == 3) {
      SDL_FPoint position = { (float)x, (float)y };
      if (add_node(graph, position) != id) {
        fprintf(stderr, "Error adding node with ID %d\n", id);
        fclose(file);
        return false;
      }
    } else {
      fprintf(stderr, "Invalid line format: %s\n", line);
    }
  }

  fclose(file);
  return true;
}

bool load_connections_from_file(Graph* graph, const char* filename) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    return false;
  }

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    int source, target;
    float length;
    char name[128];
    if (sscanf(line, "%d,%d,%f,%s", &source, &target, &length, name) == 4) {
      float friction = 0.1f + ((float)rand() / RAND_MAX) * (0.9f - 0.1f);
      if (!add_connection(graph, source, target, length, friction, name)) {
        fprintf(stderr, "Error adding connection from %d to %d\n", source, target);
        fclose(file);
        return false;
      }
    } else {
      fprintf(stderr, "Invalid line format: %s\n", line);
    }
  }

  fclose(file);
  return true;
}