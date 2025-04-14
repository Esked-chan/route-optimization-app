#ifndef IMPORT_H
#define IMPORT_H

#include "graph.h"

bool load_nodes_from_file(Graph* graph, const char* filename);
bool load_connections_from_file(Graph* graph, const char* filename);

#endif // IMPORT_H
