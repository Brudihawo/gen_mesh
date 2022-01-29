#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define VERB_LEVEL VERB_ERR
#include "logging.h"
#include "qtree.h"

#define AREA_WIDTH 10.0
#define AREA_HEIGHT 10.0

#define RED "\033[1;31m"
#define GRN "\033[1;32m"
#define RST "\033[0m"

// success if c is true
#define TEST_SUCCESS_FAILURE(c) c;\
  fprintf(stderr, "%s%s: %s %s:%i\n" RST, c ? GRN : RED, c ? "PASSED" : "FAILED", \
    __FUNCTION__, __FILE__, __LINE__)

// Some test constants
#define NODE_LOG_SIZE (12 + 2 + 12 + 1 + 4 + 10 + 1)
#define N_INSERTIONS 1024
#define N_POINTS_CLOSEST 128
#define N_TESTS_CLOSEST 128

float rand_float() {
  return (float)rand() / (float)RAND_MAX;
}

void qtree_traverse_node_to_buf(char* buf, size_t buf_size, size_t *cur_idx, Node* node, RelPos pos) {
  if (node->type == NODE_BRANCH || node->type == NODE_ROOT){
    for (size_t pos = RELPOS_UR; pos < RELPOS_NUM; pos++) {
      if (node->children == NULL) break;
      if (&node->children[pos] != NULL) {
        qtree_traverse_node_to_buf(buf, buf_size, cur_idx, &node->children[pos], pos);
      }
    }
  }
  if (node->type == NODE_LEAF || node->type == NODE_EMPTY) {
    snprintf(&buf[*cur_idx], buf_size - *cur_idx, "%12s: %12s (%5.2f, %5.2f)\n",
        relpos_to_cstr(pos),
        node_type_to_cstr(node->type),
        node->pos.x,
        node->pos.y
        );
    *cur_idx += NODE_LOG_SIZE;
  }
}

int test_qtree_insert_number(void) {
  V2 mid = v2(0, 0);
  Node root = node_new(mid, NODE_ROOT, AREA_WIDTH, AREA_HEIGHT);

  for (size_t i = 0; i < N_INSERTIONS; i++) {
    float x = (float)rand() / (float)RAND_MAX * AREA_WIDTH / 2;
    float y = (float)rand() / (float)RAND_MAX * AREA_HEIGHT / 2;
    qtree_insert(&root, v2(x, y));
  }

  size_t leaves = qtree_count_leaves(&root);
  int suc = TEST_SUCCESS_FAILURE(leaves == N_INSERTIONS);
  if (!suc) {
    fprintf(stderr, "-> Expected %ld leaves, got %ld.\n", (long)N_INSERTIONS, leaves);
  }

  qtree_free(&root);
  return suc;
}

// TODO: Add assert statement for verification
int test_qtree_insert_same(void) {
  V2 mid = v2(0, 0);
  Node root = node_new(mid, NODE_ROOT, AREA_WIDTH, AREA_HEIGHT);
  qtree_insert(&root, v2(1, 1));
  qtree_insert(&root, v2(1, 1));

  size_t bufsize = NODE_LOG_SIZE * 4 + 1;
  char write_buf[bufsize];
  size_t cur_idx = 0;
  qtree_traverse_node_to_buf(write_buf, bufsize, &cur_idx, &root, 0);

  const char* expected =  " UPPER RIGHT:   NODE_EMPTY ( 2.50, -2.50)\n"
                          "  UPPER LEFT:   NODE_EMPTY (-2.50, -2.50)\n"
                          "  LOWER LEFT:   NODE_EMPTY (-2.50,  2.50)\n"
                          " LOWER RIGHT:    NODE_LEAF ( 1.00,  1.00)\n";
  int suc = TEST_SUCCESS_FAILURE(!strncmp(expected, write_buf, bufsize));
  if (!suc) {
    fprintf(stderr, "-> Expected\n%s", expected);
    fprintf(stderr, "-> Actual\n%s", write_buf);
  }

  qtree_free(&root);
  return suc;
}

int test_qtree_insert_pos(void) {
  V2 mid = v2(0.0, 0.0);
  Node root = node_new(mid, NODE_ROOT, AREA_WIDTH, AREA_HEIGHT);
  V2 children[] = {
    [RELPOS_UR] = v2( 2.5, -2.5),
    [RELPOS_UL] = v2(-2.5, -2.5),
    [RELPOS_LL] = v2(-2.5,  2.5),
    [RELPOS_LR] = v2( 2.5,  2.5),
  };

  for (size_t i = 0; i < 4; i++) {
    qtree_insert(&root, children[i]);
  }

  // Output Traversal to buffer
  size_t bufsize = 4 * NODE_LOG_SIZE + 1;
  char write_buf[bufsize];
  size_t cur_idx = 0;
  qtree_traverse_node_to_buf(write_buf, bufsize, &cur_idx, &root, RELPOS_NUM);

  const char* expected = " UPPER RIGHT:    NODE_LEAF ( 2.50, -2.50)\n"
                         "  UPPER LEFT:    NODE_LEAF (-2.50, -2.50)\n"
                         "  LOWER LEFT:    NODE_LEAF (-2.50,  2.50)\n"
                         " LOWER RIGHT:    NODE_LEAF ( 2.50,  2.50)\n";

  int suc = TEST_SUCCESS_FAILURE(!(bool)strncmp(expected, write_buf, bufsize));
  if (!suc) {
    fprintf(stderr, "-> Expected\n%s", expected);
    fprintf(stderr, "-> Actual\n%s", write_buf);
  }

  qtree_free(&root);
  return suc;
}

int test_closest_location() {
  float xs[N_POINTS_CLOSEST];
  float ys[N_POINTS_CLOSEST];

  V2 mid = v2(0.0, 0.0);
  Node root = node_new(mid, NODE_ROOT, 1.0, 1.0);

  // TODO: Assert fails in qtree insertion, why?
  for (size_t i = 0; i < N_POINTS_CLOSEST; i++) {
    xs[i] = rand_float();
    ys[i] = rand_float();
    qtree_insert(&root, v2(xs[i], ys[i]));
  }

  int suc = true;
  for (size_t i = 0; i < N_TESTS_CLOSEST && suc; i++) {
    V2 c_pt = v2(rand_float(), rand_float());
    Node* c_node = qtree_find_closest(&root, &c_pt);
    V2 c_pos = qtree_find_closest(&root, &c_pt)->pos;
    float closest_dist = v2_dist(c_pt, c_pos);
    for (size_t i = 0; i < N_POINTS_CLOSEST && suc; i++) {
      float cur_dist = v2_dist(c_pt, v2(xs[i], ys[i]));
      if (cur_dist < closest_dist) suc = false;
    }
  }

  qtree_free(&root);
  return suc;
}

// TODO: Test insert position location correctness
// TODO: Test closest point correctness

typedef int (test_func)(void);
bool exec_test(test_func func) {
  bool suc = func();
  if (!suc) fprintf(stderr, "==============================================\n");
  return suc;
}

int main() {
  srand(0x69);
  test_func *functions[] = {
    &test_qtree_insert_number,
    &test_qtree_insert_pos,
    &test_qtree_insert_same,
    &test_closest_location,
  };

  size_t n_funcs = sizeof(functions)/(sizeof(functions[0]));
  bool success = true;
  for (size_t i = 0; i < n_funcs && success; i++) {
    if (!exec_test(functions[i])) break;
  }
}
