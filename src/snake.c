#include "snake.h"
#include "aulib_wasm.h"
#include "colors.h"
#include "keys.h"

#define COLOR_FOOD 0xf85149
#define COLOR_GRID 0x121820

#define GRID_W 16
#define GRID_H 16
#define CELL_PX 18
#define MAX_SNAKE 256

#define SNAKE_TICK_START 24
#define SNAKE_TICK_MIN 4

typedef struct {
  int x;
  int y;
} Pt;

static Pt snake_body[MAX_SNAKE];
static int snake_len = 0;
static int snake_dx = 1;
static int snake_dy = 0;
static int pending_dx = 1;
static int pending_dy = 0;
static Pt food = {5, 5};
static int snake_alive = 0;
static int snake_paused = 0;
static int snake_score = 0;
static int snake_started = 0;
static int snake_tick_frames = SNAKE_TICK_START;
static int frame_counter = 0;

static int cell_occupied(int x, int y) {
  for (int i = 0; i < snake_len; i++) {
    if (snake_body[i].x == x && snake_body[i].y == y) {
      return 1;
    }
  }
  return 0;
}

static void place_food(void) {
  for (int tries = 0; tries < 256; tries++) {
    int fx = rand_below(GRID_W);
    int fy = rand_below(GRID_H);
    if (!cell_occupied(fx, fy)) {
      food.x = fx;
      food.y = fy;
      return;
    }
  }
}

static void update_score(void) {
  set_text("#snake-score", au_format("Score: %d", snake_score));
}

static void reset_snake(void) {
  snake_body[0].x = 8;
  snake_body[0].y = 8;
  snake_body[1].x = 7;
  snake_body[1].y = 8;
  snake_body[2].x = 6;
  snake_body[2].y = 8;
  snake_len = 3;
  snake_dx = 1;
  snake_dy = 0;
  pending_dx = 1;
  pending_dy = 0;
  snake_score = 0;
  snake_tick_frames = SNAKE_TICK_START;
  frame_counter = 0;
}

static void reset_game(void) {
  int reroll_food = snake_started || cell_occupied(food.x, food.y);
  reset_snake();
  if (reroll_food) {
    place_food();
  }
  update_score();
  snake_alive = 1;
  snake_paused = 0;
  snake_started = 1;
  set_text("#snake-status", "Playing");
}

static void step(void) {
  if (!snake_started || !snake_alive || snake_paused) {
    return;
  }

  snake_dx = pending_dx;
  snake_dy = pending_dy;

  int nx = snake_body[0].x + snake_dx;
  int ny = snake_body[0].y + snake_dy;

  if (nx < 0 || nx >= GRID_W || ny < 0 || ny >= GRID_H) {
    snake_alive = 0;
    set_text("#snake-status", au_format("Game over - score %d", snake_score));
    return;
  }

  for (int i = 0; i < snake_len - 1; i++) {
    if (snake_body[i].x == nx && snake_body[i].y == ny) {
      snake_alive = 0;
      set_text("#snake-status", au_format("Game over - score %d", snake_score));
      return;
    }
  }

  int ate = (nx == food.x && ny == food.y);
  if (ate && snake_len < MAX_SNAKE) {
    snake_len++;
  }

  for (int i = snake_len - 1; i > 0; i--) {
    snake_body[i] = snake_body[i - 1];
  }
  snake_body[0].x = nx;
  snake_body[0].y = ny;

  if (ate) {
    snake_score++;
    update_score();
    if (snake_tick_frames > SNAKE_TICK_MIN) {
      snake_tick_frames--;
    }
    place_food();
  }
}

static void render(void) {
  canvas_clear("#snake-canvas", COLOR_BG);

  for (int x = 0; x < GRID_W; x++) {
    canvas_fill_rect("#snake-canvas", x * CELL_PX, 0, 1, GRID_H * CELL_PX,
                     COLOR_GRID);
  }
  for (int y = 0; y < GRID_H; y++) {
    canvas_fill_rect("#snake-canvas", 0, y * CELL_PX, GRID_W * CELL_PX, 1,
                     COLOR_GRID);
  }

  canvas_fill_rect("#snake-canvas", food.x * CELL_PX + 3, food.y * CELL_PX + 3,
                   CELL_PX - 6, CELL_PX - 6, COLOR_FOOD);

  for (int i = 0; i < snake_len; i++) {
    unsigned int color = (i == 0) ? COLOR_ACCENT : COLOR_ACCENT_DIM;
    canvas_fill_rect("#snake-canvas", snake_body[i].x * CELL_PX + 1,
                     snake_body[i].y * CELL_PX + 1, CELL_PX - 2, CELL_PX - 2,
                     color);
  }
}

static void on_key_press(int code) {
  if (code == KEY_SPACE) {
    if (!snake_started || !snake_alive) {
      reset_game();
    } else {
      snake_paused = !snake_paused;
      set_text("#snake-status", snake_paused ? "Paused" : "Playing");
    }
    return;
  }

  if (!snake_alive || snake_paused) {
    return;
  }

  if (key_is_up(code) && snake_dy == 0) {
    pending_dx = 0;
    pending_dy = -1;
  } else if (key_is_down(code) && snake_dy == 0) {
    pending_dx = 0;
    pending_dy = 1;
  } else if (key_is_left(code) && snake_dx == 0) {
    pending_dx = -1;
    pending_dy = 0;
  } else if (key_is_right(code) && snake_dx == 0) {
    pending_dx = 1;
    pending_dy = 0;
  }
}

static void on_start_click(void) { reset_game(); }

static void on_pause_click(void) {
  if (!snake_started) {
    return;
  }
  if (!snake_alive) {
    reset_game();
    return;
  }
  snake_paused = !snake_paused;
  set_text("#snake-status", snake_paused ? "Paused" : "Playing");
}

void snake_init(void) {
  on_click("#snake-start", on_start_click);
  on_click("#snake-pause", on_pause_click);
  on_key(on_key_press);
  reset_snake();
  place_food();
  update_score();
  render();
}

void snake_frame(void) {
  if (++frame_counter >= snake_tick_frames) {
    frame_counter = 0;
    step();
  }
  render();
}
