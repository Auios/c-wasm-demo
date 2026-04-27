#include "aulib_wasm.h"

static int count = 0;

static void sync_tap_display(void) {
  set_text("#tap-count", au_format("%d", count));
}

static void handle_tap(void) {
  count++;
  sync_tap_display();
  set_text("#status", au_format("Taps: %d · last: +1", count));
}

static void handle_reset(void) {
  count = 0;
  sync_tap_display();
  set_text("#status", "Counter reset from C");
}

static void handle_double(void) {
  if (count != 0 && count <= 1073741823) {
    count *= 2;
  }
  sync_tap_display();
  set_text("#status", au_format("Doubled → %d", count));
}

static void handle_add_ten(void) {
  if (count <= 2147483637) {
    count += 10;
  }
  sync_tap_display();
  set_text("#status", au_format("Added 10 → %d", count));
}

static void handle_clear(void) {
  count = 0;
  sync_tap_display();
  set_text("#status", "Cleared (host DOM unchanged)");
}

static void handle_range(void) {
  int v = get_input_int("#demo-range");
  set_text("#range-value", au_format("%d", v));
  set_text("#status", au_format("Slider: %d", v));
}

__attribute__((export_name("init"))) void init(void) {
  set_text("#status", "Hello from C — aulib-wasm is live");
  sync_tap_display();
  set_text("#range-value", au_format("%d", get_input_int("#demo-range")));

  on_click("#btn", handle_tap);
  on_click("#btn-reset", handle_reset);
  on_click("#btn-double", handle_double);
  on_click("#btn-ten", handle_add_ten);
  on_click("#btn-clear", handle_clear);
  on_input("#demo-range", handle_range);
}
