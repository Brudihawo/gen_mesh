#include "stdlib.h"
#include "stdbool.h"

#define POINTS_CAP 1024

typedef struct {
  float x;
  float y;
} V2;

/****************************************************
 * PList is a list of points with fixed capacity.
 * Pushing and Popping is possible.
 * Size does not need to be known at compile time
 */
typedef struct {
  V2 *points;
  size_t count;
} PList;


PList PList_new(size_t points_cap);
void PList_free(PList* list);
bool PList_push(PList* list, float x, float y);
bool PList_pop(PList* list);

/****************************************************
 * EList is a list of edges with fixed capacity.
 * Pushing and Popping is possible.
 * Size does not need to be known at compile time
 */
typedef struct {
  V2 *p0;
  V2 *p1;
} Edge;

typedef struct {
  Edge *edges;
  size_t count;
} EList;

EList EList_new(size_t edges_cap);
void EList_free(EList* list);
bool EList_push(EList* list, V2* p0, V2* p1);
bool EList_pop(PList* list);
