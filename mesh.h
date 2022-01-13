#ifndef MESH_H
#define MESH_H
#include "datastructs.h"

/****************************************************
 * A Mesh is made from Cells, which in turn consist 
 * of 3 Points and 3 (optional) indices of neighbors.
 */

typedef struct {
  V2 *points[3];
  int64_t neighbors[3];
} Cell;

typedef struct {
  Cell *cells;
  size_t count;
  size_t cap;
} Mesh;

Mesh Mesh_new(size_t cell_cap);
void Mesh_free(Mesh* msh);
bool Mesh_push(Mesh* msh, V2* p0, V2* p1, V2* p2, int64_t neighbors[3]);
bool Mesh_pop(Mesh* msh);
#endif // MESH_H
