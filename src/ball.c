#include "ball.h"
#include "anim.h"
#include "aulib_wasm.h"
#include "colors.h"

#define BALL_R 14
#define BALL_GRAVITY_PX_S2 1000.0f
#define BALL_BOUNCE_X 0.85f /* fraction of |vx| kept per side-wall hit */
#define BALL_BOUNCE_Y 0.85f /* fraction of |vy| kept per top/floor hit */
#define BALL_FLOOR_DRAG_PER_S 0.6f /* fraction of vx lost per second on floor */

#define BALL_KICK_MIN_S 3.0f
#define BALL_KICK_MAX_S 10.0f
#define BALL_FLASH_S 0.2f

#define BALL_KICK_HORIZ_MAX_PX_S 300.0f
#define BALL_KICK_UP_MIN_PX_S 260.0f
#define BALL_KICK_UP_MAX_PX_S 540.0f

#define BALL_KICK_MIN_FRAMES ((int)(BALL_KICK_MIN_S * ANIM_FPS))
#define BALL_KICK_MAX_FRAMES ((int)(BALL_KICK_MAX_S * ANIM_FPS))
#define BALL_FLASH_FRAMES ((int)(BALL_FLASH_S * ANIM_FPS))

#define COLOR_BALL_KICK 0xf6c860

static float ball_x = 60.0f;
static float ball_y = 60.0f;
static float ball_vx = 240.0f; /* px/s */
static float ball_vy = 0.0f;   /* px/s */
static int ball_w = 0;
static int ball_h = 0;
static int ball_kick_in = 90;
static int ball_kick_flash = 0;

static void ball_kick(void) {
  /* Uniform horizontal in [-MAX, +MAX] px/s. */
  float h = ((float)rand_below(2001) - 1000.0f) / 1000.0f;
  ball_vx += h * BALL_KICK_HORIZ_MAX_PX_S;

  /* Uniform upward in [UP_MIN, UP_MAX] px/s. */
  float u = (float)rand_below(1001) / 1000.0f;
  ball_vy -= BALL_KICK_UP_MIN_PX_S +
             u * (BALL_KICK_UP_MAX_PX_S - BALL_KICK_UP_MIN_PX_S);

  ball_kick_flash = BALL_FLASH_FRAMES;
}

void ball_init(void) {
  /* Nothing to wire; state defaults are fine. */
}

void ball_frame(void) {
  if (ball_w == 0 || ball_h == 0) {
    ball_w = canvas_width("#ball-canvas");
    ball_h = canvas_height("#ball-canvas");
    if (ball_w == 0 || ball_h == 0) {
      return;
    }
  }

  ball_vy += BALL_GRAVITY_PX_S2 * ANIM_DT;

  ball_x += ball_vx * ANIM_DT;
  ball_y += ball_vy * ANIM_DT;

  if (ball_x - BALL_R < 0) {
    ball_x = (float)BALL_R;
    ball_vx = -ball_vx * BALL_BOUNCE_X;
  }
  if (ball_x + BALL_R > (float)ball_w) {
    ball_x = (float)(ball_w - BALL_R);
    ball_vx = -ball_vx * BALL_BOUNCE_X;
  }
  if (ball_y - BALL_R < 0) {
    ball_y = (float)BALL_R;
    ball_vy = -ball_vy * BALL_BOUNCE_Y;
  }
  if (ball_y + BALL_R > (float)ball_h) {
    ball_y = (float)(ball_h - BALL_R);
    ball_vy = -ball_vy * BALL_BOUNCE_Y;
    ball_vx -= ball_vx * BALL_FLOOR_DRAG_PER_S * ANIM_DT;
  }

  if (--ball_kick_in <= 0) {
    ball_kick();
    ball_kick_in = BALL_KICK_MIN_FRAMES +
                   rand_below(BALL_KICK_MAX_FRAMES - BALL_KICK_MIN_FRAMES);
  }

  unsigned int color = ball_kick_flash > 0 ? COLOR_BALL_KICK : COLOR_ACCENT;
  if (ball_kick_flash > 0) {
    ball_kick_flash--;
  }

  canvas_clear("#ball-canvas", COLOR_BG);
  canvas_fill_circle("#ball-canvas", (int)ball_x, (int)ball_y, BALL_R, color);
}
