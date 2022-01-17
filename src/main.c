#include "stdio.h"
#include "stdbool.h"
#include "assert.h"

#include "SDL.h"

#include "datastructs.h"
#include "qtree.h"
#include "mesh.h"
#include "logging.h"


#define UNPACK(val) (val & 0xFF000000) >> 24,\
                    (val & 0x00FF0000) >> 16,\
                    (val & 0x0000FF00) >> 8,\
                    (val & 0x000000FF)

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define C_BACK 0x352F2AFF
#define C_PNTS 0xC65333FF
#define C_OUTL 0x99D59DFF
#define C_OUTL_STA 0xB380B0FF
#define C_OUTL_END 0x729893FF
#define C_QTREE_EMPTY 0x485F84FF
#define C_QTREE_LEAF  0xE49B5DFF
#define C_QTREE_ROBR  0x7D2A2FFF

#define POINTS_DRAW_RADIUS 10 // pixels

#define POINTS_CAP 1024


void draw_rect(SDL_Renderer* renderer, float x, float y, float w, float h, bool fill) {
  const SDL_FRect rect = {
    .x = x - w / 2,
    .y = y - h / 2,
    .w = w,
    .h = h
  };
  if (fill) {
    SDL_RenderFillRectF(renderer, &rect);
  } else {
    SDL_RenderDrawRectF(renderer, &rect);
  }
}

typedef enum {
  MODE_POINTS = 0,
  MODE_OUTLINE,
  MODE_SELECT,
  N_MODES
} ProgramMode;

PList g_points;
PList outline;

EList edges;
Node qtree;
Mesh mesh;
bool draw_tree = false;

ProgramMode mode = MODE_OUTLINE;
SDL_Renderer *renderer;

void draw_qtree(Node* node) {
  switch (node->type) {
    case NODE_BRANCH:
      SDL_SetRenderDrawColor(renderer, UNPACK(C_QTREE_ROBR));
      draw_rect(renderer, P_COORDS(node->pos),
                POINTS_DRAW_RADIUS, POINTS_DRAW_RADIUS, true);
      draw_rect(renderer, P_COORDS(node->pos),
                node->w, node->h, false);
      for (size_t pos = RELPOS_UR; pos < RELPOS_NUM; pos++) {
        draw_qtree(&node->children[pos]);
      }
      break;
    case NODE_LEAF:
      SDL_SetRenderDrawColor(renderer, UNPACK(C_QTREE_LEAF));
      draw_rect(renderer, P_COORDS(node->pos),
                POINTS_DRAW_RADIUS, POINTS_DRAW_RADIUS, true);
      break;
    case NODE_EMPTY:
      SDL_SetRenderDrawColor(renderer, UNPACK(C_QTREE_EMPTY));
      draw_rect(renderer, P_COORDS(node->pos), node->w, node->h, false);
      draw_rect(renderer, P_COORDS(node->pos),
                POINTS_DRAW_RADIUS, POINTS_DRAW_RADIUS, true);
      break;
    case NODE_ROOT:
      SDL_SetRenderDrawColor(renderer, UNPACK(C_QTREE_ROBR));
      draw_rect(renderer, P_COORDS(node->pos),
                POINTS_DRAW_RADIUS, POINTS_DRAW_RADIUS, true);
      if (node->children != NULL) {
        for (size_t pos = RELPOS_UR; pos < RELPOS_NUM; pos++) {
          draw_qtree(&node->children[pos]);
        }
      }
      break;
  }
}

// build a qtree form n_lists PLists
void build_qtree(Node* root, size_t n_lists, ...) {
  va_list args;
  va_start(args, n_lists);

  for (size_t i = 0; i < n_lists; i++) {
    PList *list = va_arg(args, PList*);
    for (size_t j = 0; j < list->count; j++) {
      qtree_insert(root, list->points[j]);
    }
  }
  va_end(args);
}

void regenerate_qtree() {
  qtree_free(&qtree);
  build_qtree(&qtree, 2, &g_points, &outline);
}

void undo() {
  switch (mode) {
    case MODE_OUTLINE:
      PList_pop(&outline);
      break;
    case MODE_POINTS:
      PList_pop(&g_points);
      break;
    case MODE_SELECT:
      break;
    case N_MODES:
      assert(false && "unreachable");
  }
}

void handle_sdlevents(bool* quit) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_d:
          draw_tree = !draw_tree;
          log_msg("Toggle Drawing Qtree");
          break;
        case SDLK_g:
          regenerate_qtree();
          break;
        case SDLK_p:
          qtree_traverse_node(&qtree);
          break;
        case SDLK_q:
          *quit = true;;
          break;
        case SDLK_u:
          undo();
          break;
        case SDLK_TAB:
          mode = (mode + 1) % N_MODES;
          log_msg("change mode to %i", mode);
          break;
      }
    }

    if (event.type == SDL_MOUSEBUTTONUP) {
      PList* cur_list = mode == MODE_OUTLINE ? &outline : &g_points;
      if (!PList_push(cur_list, event.button.x, event.button.y)) {
        log_msg("WARN: Point capacity Reached");
      }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN) {
    }

    if (event.type == SDL_QUIT) {
      *quit = true;
    }
  }
}

void draw_outline() {
  SDL_SetRenderDrawColor(renderer, UNPACK(C_OUTL));
  SDL_RenderDrawLinesF(renderer, (const SDL_FPoint*) outline.points, outline.count);
  SDL_RenderDrawLineF(renderer, P_COORDS(outline.points[0]), P_COORDS(outline.points[outline.count - 1]));
  SDL_SetRenderDrawColor(renderer, UNPACK(C_OUTL_STA));
  draw_rect(renderer, P_COORDS(outline.points[0]),
            POINTS_DRAW_RADIUS, POINTS_DRAW_RADIUS, true);
  SDL_SetRenderDrawColor(renderer, UNPACK(C_OUTL_END));
  draw_rect(renderer, P_COORDS(outline.points[outline.count - 1]),
            POINTS_DRAW_RADIUS, POINTS_DRAW_RADIUS, true);
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    log_msg("ERROR: Failed to initialise SDL!");
    exit(1);
  }

  SDL_Window *window = NULL;
  window = SDL_CreateWindow("Generate Mesh", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);

  if (window == NULL) {
    log_msg("ERROR: Window could not be created!");
    exit(1);
  }

  renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
  bool quit = false;

  g_points = PList_new(POINTS_CAP);
  outline = PList_new(POINTS_CAP);

  qtree = node_new(v2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2),
                   NODE_ROOT,
                   SCREEN_WIDTH, SCREEN_HEIGHT);

  log_msg("Root of Qtree at (%.2f, %.2f) with dims (%.2f, %.2f)",
          P_COORDS(qtree.pos), qtree.w, qtree.h);

  while (!quit) {
    handle_sdlevents(&quit);

    SDL_SetRenderDrawColor(renderer, UNPACK(C_PNTS));
    for (size_t i = 0; i < g_points.count; i++) {
      draw_rect(renderer, g_points.points[i].x, g_points.points[i].y,
                POINTS_DRAW_RADIUS, POINTS_DRAW_RADIUS, true);
    }

    draw_outline();

    if (draw_tree) {
      draw_qtree(&qtree);
    }

    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, UNPACK(C_BACK));
    SDL_RenderClear(renderer);
  }

  PList_free(&g_points);
  PList_free(&outline);

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
