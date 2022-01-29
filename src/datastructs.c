#include "datastructs.h"

#include "stdio.h"
#include "stdbool.h"
#include "math.h"
#include "assert.h"

inline V2 v2(float x, float y) {
  return (V2) {.x = x, .y = y};
}

inline V2 v2_add(const V2 a, const V2 b) {
  return (V2) {a.x + b.x, a.y + b.y};
}

inline V2 v2_sub(const V2 a, const V2 b) {
  return (V2) {a.x - b.x, a.y - b.y};
}

inline V2 v2_scale(const V2 a, float s) {
  return (V2) {a.x * s, a.y * s};
}

inline float v2_len(const V2 a) {
  return sqrt(a.x * a.x + a.y * a.y);
}

inline float v2_dist(const V2 a, const V2 b) {
  return v2_len(v2_sub(b, a));
}

bool v2_eq(const V2 a, const V2 b) {
  return (a.x == b.x) && (a.y == b.y);
}

RelPos relative_pos(const V2* target, const V2* rel) {
  // TODO: maybe condense this using setting of bits
  if        (rel->x <= target->x && rel->y <= target->y) {
    return RELPOS_UL;
  } else if (rel->x <= target->x && rel->y  > target->y) {
    return RELPOS_LL;
  } else if (rel->x  > target->x && rel->y <= target->y) {
    return RELPOS_UR;
  } else if (rel->x  > target->x && rel->y  > target->y) {
    return RELPOS_LR;
  }
  assert(false && "unreachable, all positions are covered, maybe a value is nan");
}

const char* relpos_to_cstr(RelPos relpos) {
  switch (relpos) {
    case RELPOS_UR:
      return "UPPER RIGHT";
    case RELPOS_UL:
      return "UPPER LEFT";
    case RELPOS_LL:
      return "LOWER LEFT";
    case RELPOS_LR:
      return "LOWER RIGHT";
    case RELPOS_NUM:
      return "NUM OF RELPOS";
  }
}


PList PList_new(size_t points_cap) {
  return (PList) {
    .points = malloc(sizeof(V2) * points_cap),
    .count = 0,
    .cap = points_cap,
  };
}

void PList_free(PList* list) {
  free(list->points);
  list->count = 0;
}

bool PList_push(PList* list, float x, float y) {
  if (list->count >= list->cap) {
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
    .edges = malloc(sizeof(V2*) * edges_cap),
    .count = 0,
    .cap = edges_cap,
  };
}

void EList_free(EList* list) {
  free(list->edges);
  list->cap = 0;
}

bool EList_push(EList* list, V2* p0, V2* p1) {
  if (list->count >= list->cap) {
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
