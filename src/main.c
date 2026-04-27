#include "aulib_wasm.h"
#include "ball.h"
#include "counter.h"
#include "range.h"
#include "snake.h"

static void on_frame(void) {
  ball_frame();
  snake_frame();
  request_frame(on_frame);
}

__attribute__((export_name("init"))) void init(void) {
  set_text("#status", "Hello world from main.c!");

  counter_init();
  range_init();
  ball_init();
  snake_init();

  request_frame(on_frame);
}
