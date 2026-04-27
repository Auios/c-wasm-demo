#pragma once

/* Standard JS keyCode values from the host (set by runtime/wasm_loader.js). */
#define KEY_SPACE 32
#define KEY_LEFT 37
#define KEY_UP 38
#define KEY_RIGHT 39
#define KEY_DOWN 40
#define KEY_A 65
#define KEY_D 68
#define KEY_S 83
#define KEY_W 87

static inline int key_is_up(int code) {
  return code == KEY_UP || code == KEY_W;
}
static inline int key_is_down(int code) {
  return code == KEY_DOWN || code == KEY_S;
}
static inline int key_is_left(int code) {
  return code == KEY_LEFT || code == KEY_A;
}
static inline int key_is_right(int code) {
  return code == KEY_RIGHT || code == KEY_D;
}
