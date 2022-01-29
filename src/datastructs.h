#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H
#include "stdlib.h"
#include "stdbool.h"

#define P_COORDS(point) point.x, point.y
typedef struct {
  float x;
  float y;
} V2;

// Position of child nodes counter clockwise
typedef enum {
  RELPOS_UR = 0,
  RELPOS_UL,
  RELPOS_LL,
  RELPOS_LR,
  RELPOS_NUM
} RelPos;

const char* relpos_to_cstr(RelPos relpos);

V2 v2(float x, float y);
V2 v2_add(const V2 a, const V2 b);
V2 v2_sub(const V2 a, const V2 b);
V2 v2_scale(const V2 a, float s);
float v2_len(const V2 a);
float v2_dist(const V2 a, const V2 b);
bool v2_eq(const V2 a, const V2 b);

RelPos relative_pos(const V2* pos, const V2* rel);

/****************************************************
 * PList is a list of points with fixed capacity.
 * Pushing and Popping is possible.
 * Size does not need to be known at compile time
 */
typedef struct {
  V2 *points;
  size_t count;
  size_t cap;
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
  size_t cap;
} EList;

EList EList_new(size_t edges_cap);
void EList_free(EList* list);
bool EList_push(EList* list, V2* p0, V2* p1);
bool EList_pop(PList* list);
#endif // DATASTRUCTS_H
