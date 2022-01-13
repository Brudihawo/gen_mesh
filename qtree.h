#ifndef QTREE_H
#define QTREE_H
#include "stdio.h"
#include "stdbool.h"
#include "assert.h"

#include "datastructs.h"


typedef struct Node_t Node;
typedef enum {
  NODE_EMPTY, // leaf node with centered position, no information 
  NODE_LEAF, // leaf node with information (position is non-centered)
  NODE_BRANCH, // 4 Children
  NODE_ROOT, // 0 or 4 Children
} NodeType;

struct Node_t {
  V2 pos;
  float w;
  float h;
  NodeType type;
  Node* children; // 4 Children or None
};

Node node_new(V2 pos, NodeType type, float w, float h);

// generate a new V2 from parent node
// with parents width and height / 2 as distance per direction
V2 gen_pos_parent(Node* parent, RelPos pos);

bool qtree_insert(Node *node, V2 point);
void qtree_traverse_node(Node* node);
void node_free(Node* node);
void qtree_free(Node* root);

#endif // QTREE_H
