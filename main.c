#include "stdio.h"
#include "stdbool.h"
#include "stdarg.h"
#include "assert.h"

#include "SDL.h"

#include "datastructs.h"

#define log_msg(...) _log(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

#define P_COORDS(point) point.x, point.y

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

#define POINTS_DRAW_RADIUS 10 // pixels

void _log(const char* func, const char *file, int line, const char *fmt, ...) {
  va_list args;
  fprintf(stderr, "%s:%i (%s): ", file, line, func);
  va_start(args, fmt);

  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");

  va_end(args);
}

void draw_rect(SDL_Renderer* renderer, int x, int y, int w, int h, bool fill) {
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
  N_MODES
} ProgramMode;


PList g_points;
PList outline;

EList edges;


ProgramMode mode = MODE_OUTLINE;
SDL_Renderer *renderer;

void undo() {
  switch (mode) {
    case MODE_OUTLINE:
      PList_pop(&outline);
      break;
    case MODE_POINTS:
      PList_pop(&g_points);
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
        case SDLK_q:
          *quit = true;
          break;
        case SDLK_u:
          undo();
        case SDLK_m:
          mode = (mode + 1) % N_MODES;
      }
    }

    if (event.type == SDL_MOUSEBUTTONUP) {
      PList* cur_list = mode == MODE_OUTLINE ? &outline : &g_points;
      if (!PList_push(cur_list, event.button.x, event.button.y)) {
        log_msg("WARN: Point capacity Reached");
      }
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

  while (!quit) {
    handle_sdlevents(&quit);

    SDL_SetRenderDrawColor(renderer, UNPACK(C_PNTS));
    for (size_t i = 0; i < g_points.count; i++) {
      draw_rect(renderer, g_points.points[i].x, g_points.points[i].y,
                POINTS_DRAW_RADIUS, POINTS_DRAW_RADIUS, true);
    }

    draw_outline();

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
