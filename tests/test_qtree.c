#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define VERB_LEVEL VERB_ERR
#include "logging.h"
#include "qtree.h"


#define AREA_WIDTH 1000.0
#define AREA_HEIGHT 1000.0

#define TEST_SUCCESS_FAILURE(condition)                                        \
  fprintf(stderr, "%s: %s\n", condition ? "PASSED" : "FAILED", __FUNCTION__)

FILE *safeopen(const char *fname, const char *mode) {
  FILE *f = fopen(fname, mode);

  if (f == NULL) {
    fprintf(stderr, "Could not open file %s: %s", fname, strerror(errno));
    exit(1);
  }

  return f;
}

typedef struct {
  V2 pos[4];
} OChildren;

OChildren gen_Ochildren(V2 pos, float xoff, float yoff) {
  assert(xoff >= 0 && yoff >= 0 && "Offsets must be larger than 0");
  return (OChildren){.pos = {
                         [RELPOS_UR] = v2_add(pos, v2(xoff, -yoff)),
                         [RELPOS_UL] = v2_add(pos, v2(-xoff, -yoff)),
                         [RELPOS_LL] = v2_add(pos, v2(-xoff, yoff)),
                         [RELPOS_LR] = v2_add(pos, v2(xoff, yoff)),
                     }};
}

void insert_ochildren(Node *root, OChildren *children) {
  for (int rpos = RELPOS_UR; rpos < RELPOS_NUM; rpos++) {
    fprintf(stderr, "INSERT AT (%.2f, %.2f)\n", P_COORDS(children->pos[rpos]));
    qtree_insert(root, children->pos[rpos]);
  }
}

void test_qtree_traverse_node() {
  V2 mid = v2(AREA_WIDTH / 2, AREA_HEIGHT / 2);
  OChildren regular_pos = gen_Ochildren(mid, 10, 10);

  Node root = node_new(mid, NODE_ROOT, AREA_WIDTH, AREA_HEIGHT);
  insert_ochildren(&root, &regular_pos);

  FILE *f = safeopen("TEST_qtree_traverse_node.txt", "w");
  _qtree_traverse_node(f, &root);
  fclose(f);
}

void test_qtree_insert_pos() {
  V2 mid = v2(0, 0);
  Node root = node_new(mid, NODE_ROOT, AREA_WIDTH, AREA_HEIGHT);
  OChildren regular_children = gen_Ochildren(root.pos, root.w / 4, root.h / 4);
  insert_ochildren(&root, &regular_children);

  for (int rpos = RELPOS_UR; rpos < RELPOS_LR; rpos++) {
    OChildren insert_children = gen_Ochildren(
        root.children[rpos].pos, root.children[rpos].w / 4, root.children[rpos].h / 4);
    insert_ochildren(&root, &insert_children);
  }
  // TODO: assertion for correctness is missing
  // qtree_traverse_node(&root);

  qtree_free(&root);
}

void test_qtree_insert_number(size_t n_insertions) {
  V2 mid = v2(0, 0);
  Node root = node_new(mid, NODE_ROOT, AREA_WIDTH, AREA_HEIGHT);

  for (size_t i = 0; i < n_insertions; i++) {
    float x = (float)rand() / (float)RAND_MAX * AREA_WIDTH / 2;
    float y = (float)rand() / (float)RAND_MAX * AREA_HEIGHT / 2;
    qtree_insert(&root, v2(x, y));
  }

  size_t leaves = qtree_count_leaves(&root);
  TEST_SUCCESS_FAILURE(leaves == n_insertions);
  fprintf(stderr, "-> Expected %ld leaves, got %ld.\n", n_insertions, leaves);

  qtree_free(&root);
}

// TODO: Add assert statement for verification
void test_qtree_insert_same() {
  V2 mid = v2(0, 0);
  Node root = node_new(mid, NODE_ROOT, AREA_WIDTH, AREA_HEIGHT);
  qtree_insert(&root, v2(10, 10));
  qtree_insert(&root, v2(10, 10));
  qtree_free(&root);
}

// TODO: Test insert position location correctness
// TODO: Test closest point correctness

int main() {
  fprintf(stderr, "VERBOSITY LEVEL: %i\n", VERB_LEVEL);
  srand(0x69);
  // test_qtree_insert_number(1024);
  // test_qtree_insert_pos();
  test_qtree_traverse_node();
  // test_qtree_insert_same();
}
