#include "mesh.h"

#include "memory.h"

Mesh Mesh_new(size_t cell_cap) {
  return (Mesh) {
    .cells = malloc(sizeof(Cell) * cell_cap),
    .count = 0,
    .cap = cell_cap
  };
}

void Mesh_free(Mesh* msh) {
  free(msh->cells);
  msh->cap = 0;
}

bool Mesh_push(Mesh* msh, V2* p0, V2* p1, V2* p2, int64_t neighbors[3]) {
  if (msh->count >= msh->cap) {
    return false;
  }

  msh->cells[msh->count] = (Cell) {
    .points = {p0, p1, p2},
  };
  memcpy((void*)&msh->cells[msh->count].neighbors, neighbors, 3 * sizeof(int64_t));
  msh->count++;

  return true;
}

bool Mesh_pop(Mesh* msh) {
  if (msh->count == 0) {
    return false;
  }
  
  msh->count--;
  return true;
}
