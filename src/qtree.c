#include "stdio.h"
#include "qtree.h"
#include "assert.h"

#include "logging.h"

Node node_new(V2 pos, NodeType type, float w, float h) {
  return (Node) {
    .pos = pos,
    .type = type, 
    .w = w, .h = h,
    .children = NULL,
  };
}

const char* node_type_to_cstr(NodeType type) {
  switch (type) {
    case NODE_ROOT:
      return "NODE_ROOT";
    case NODE_BRANCH:
      return "NODE_BRANCH";
    case NODE_LEAF:
      return "NODE_LEAF";
    case NODE_EMPTY:
      return "NODE_EMPTY";
  }
}

void _qtree_traverse_node(FILE* f, Node* node) { // counter clockwise starting at upper right
  if (node->type == NODE_BRANCH || node->type == NODE_ROOT){
    for (size_t pos = RELPOS_UR; pos < RELPOS_NUM; pos++) {
      if (&node->children[pos] != NULL) {
        _qtree_traverse_node(f, &node->children[pos]);
      }
    }
  }
  if (node->type == NODE_LEAF || node->type == NODE_EMPTY) {
    fprintf(f, "%s at %.2f %.2f with dimensions %.2f, %.2f\n",
           node_type_to_cstr(node->type),
           node->pos.x, node->pos.y, node->w, node->h);
  }
}

size_t _qtree_count_leaves(Node* node, size_t cur) {
  size_t tmp = 0;
  switch (node->type) {
  case NODE_EMPTY:
    return 0;
  case NODE_LEAF:
    return 1;
  case NODE_BRANCH ... NODE_ROOT:
    if (node->children == NULL) {
      return 0;
    }
    for (int rpos = RELPOS_UR; rpos < RELPOS_NUM; rpos++) {
      tmp += _qtree_count_leaves(&node->children[rpos], cur);
    }
    return tmp;
  }
}


void qtree_free(Node* node) {
  if (node->type == NODE_BRANCH || node->type == NODE_ROOT) {
    if (node->children != NULL) {
      for (int rpos = RELPOS_UR; rpos < RELPOS_NUM; rpos++) {
        qtree_free(&node->children[rpos]);
      }
      node->children = NULL;
    }
  }
}

void node_free(Node* node) {
  if (node->children != NULL) {
    free(node->children);
  }
}

void insert_children(Node* node) {
  node->children = (Node*) malloc(4 * sizeof(Node));
  for (size_t rpos = RELPOS_UR; rpos < RELPOS_NUM; rpos++) {
    node->children[rpos] = node_new(
      gen_pos_parent(node, rpos),
      NODE_EMPTY,
      node->w / 2, node->h / 2
    );
  }
}

bool _qtree_insert(Node *ins_node, V2 point, size_t depth) {
  log_msg("Insert (%.2f, %.2f) into tree at (%.2f, %.2f) (%s) with depth %ld", 
          P_COORDS(point), P_COORDS(ins_node->pos),
          relpos_to_cstr(relative_pos(&ins_node->pos, &point)),
          depth);
  Node *cur_node = ins_node;
  Node *parent = ins_node;
  if ( (point.x > cur_node->pos.x + cur_node->w / 2)
    || (point.x < cur_node->pos.x - cur_node->w / 2)
    || (point.y > cur_node->pos.y + cur_node->h / 2)
    || (point.y < cur_node->pos.y - cur_node->h / 2)) {
    // Node out of bounds

    log_msg("Node at (%.2f, %.2f) out of bounds", point.x, point.y);
    log_msg("For parent at (%.2f, %.2f) with dims (%.2f, %.2f)!",
        cur_node->pos.x, cur_node->pos.y, cur_node->w, cur_node->h);
    return false;
  }

  // here, cur_node and parent are NODE_ROOT or NODE_BRANCH
  while (cur_node->type == NODE_ROOT || cur_node->type == NODE_BRANCH) {
    if (cur_node->children == NULL) {
      if (cur_node->type == NODE_BRANCH) {
        assert(false && "Node type cannot be branch and have NULL as children");
      }
      log_msg("Inserting children (At root)");
      insert_children(cur_node);
    }

    RelPos rpos = relative_pos(&cur_node->pos, &point);
    log_msg("Point at (%.2f, %.2f) is %s of %s at (%.2f, %.2f)",
            P_COORDS(point),
            relpos_to_cstr(rpos),
            node_type_to_cstr(cur_node->type),
            P_COORDS(cur_node->pos));
    parent = cur_node;

    // HERE, CUR_NODE can become LEAF or EMPTY
    cur_node = &cur_node->children[rpos];
    depth++;
  }


  if (cur_node->type == NODE_EMPTY) { // no data at node
      if (!(parent->type == NODE_BRANCH || parent->type == NODE_ROOT)) {
        assert(false && "Must be child of branch");
      }
      cur_node->type = NODE_LEAF;
      cur_node->pos = point;
      return true;
  } else if (cur_node->type == NODE_LEAF) { // data at node
    // insert point as new leaf
    if (v2_eq(cur_node->pos, point)) {
      log_wrn("IGNORING NODE AT (%.2f, %.2f)", P_COORDS(point));
      return false;
    }
    cur_node->type = NODE_BRANCH;
    cur_node->w = parent->w / 2;
    cur_node->h = parent->h / 2;

    // this position will one of the children's position
    // the other one will be point
    const V2 prev_node_pos = cur_node->pos;

    // insert current node as new leaf
    const RelPos cur_node_direction = relative_pos(&parent->pos, &cur_node->pos);
    cur_node->pos = gen_pos_parent(parent, cur_node_direction);

    if (cur_node->children != NULL) {
      assert(false && "This node should not have children yet");
    }
    // Initialize children at default empty position
    insert_children(cur_node);
    
    // insert node of current position
    const RelPos prev_node_rpos = relative_pos(&cur_node->pos, &prev_node_pos);
    cur_node->children[prev_node_rpos].pos = prev_node_pos;
    cur_node->children[prev_node_rpos].type = NODE_LEAF;

    return _qtree_insert(cur_node, point, depth++);
  } else {
    assert(false && "unreachable, other types handled before");
  }
}

Node* find_closest(Node* root, V2* point) {
  Node* cur_node = root;
  Node* parent = root;

  if (root->type == NODE_LEAF || root->type == NODE_EMPTY) {
    log_msg("WARN: cannot find closest node using leaf node as entry point");
    return NULL;
  }

  while (!(cur_node->type == NODE_LEAF || cur_node->type == NODE_EMPTY)) {
    RelPos rpos = relative_pos(&cur_node->pos, point);

    // Here, CUR_NODE can become NODE_LEAF or NODE_EMPTY
    parent = cur_node;
    cur_node = &cur_node->children[rpos];
  }

  Node* closest = NULL;
  float closest_dist = -1;
  for (int rpos = RELPOS_UR; rpos < RELPOS_NUM; rpos++) { // max 4 iterations
    if (parent->children[rpos].type == NODE_LEAF) { // current child is not NULL
      float cur_dist = v2_dist(closest->pos, *point);
      if (closest_dist > cur_dist) { // if no distance set yet, this will be true
        closest = &parent->children[rpos];
        closest_dist = cur_dist;
      } 
    }
  }

  return closest;
}

V2 gen_pos_parent(Node* parent, RelPos pos) {
  switch (pos) {
    case RELPOS_UR:
      return v2_add(parent->pos, v2_scale(v2(parent->w, -parent->h), 0.25));
    case RELPOS_UL:
      return v2_add(parent->pos, v2_scale(v2(-parent->w, -parent->h), 0.25));
    case RELPOS_LL:
      return v2_add(parent->pos, v2_scale(v2(-parent->w, parent->h), 0.25));
    case RELPOS_LR:
      return v2_add(parent->pos, v2_scale(v2(parent->w, parent->h), 0.25));
    default:
      assert(false && "unreachable, RELPOS_NUM is never used");
  }
}

