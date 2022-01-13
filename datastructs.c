#include "datastructs.h"

#include "stdbool.h"

PList PList_new(size_t points_cap) {
  return (PList) {
    malloc(sizeof(V2) * points_cap),
    0,
  };
}

void PList_free(PList* list) {
  free(list->points);
  list->count = 0;
}

bool PList_push(PList* list, float x, float y) {
  if (list->count >= POINTS_CAP) {
    return false;
  }
  list->points[list->count] = (V2) {x, y};
  list->count++;
  return true;
}

bool PList_pop(PList* list) {
  if (list->count == 0) {
    return false;
  }
  list->count--;
  return true;
}

EList EList_new(size_t edges_cap) {
  return (EList) {
    malloc(sizeof(V2*) * edges_cap),
    0,
  };
}

void EList_free(EList* list) {
  free(list->edges);
  list->count = 0;
}

bool EList_push(EList* list, V2* p0, V2* p1) {
  if (list->count >= POINTS_CAP) {
    return false;
  }
  list->edges[list->count] = (Edge) {p0, p1};
  list->count++;
  return true;
}

bool EList_pop(PList* list) {
  if (list->count == 0) {
    return false;
  }
  list->count--;
  return true;
}
