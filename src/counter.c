#include "counter.h"
#include "aulib_wasm.h"

static int count = 0;

static void sync_display(void) {
  set_text("#tap-count", au_format("%d", count));
}

static void on_tap(void) {
  count++;
  sync_display();
  set_text("#status", au_format("Taps: %d", count));
}

static void on_reset(void) {
  count = 0;
  sync_display();
  set_text("#status", "Counter reset!");
}

static void on_double(void) {
  if (count != 0 && count <= 1073741823) {
    count *= 2;
  }
  sync_display();
  set_text("#status", au_format("Doubled -> %d", count));
}

static void on_add_ten(void) {
  if (count <= 2147483637) {
    count += 10;
  }
  sync_display();
  set_text("#status", au_format("Added 10 -> %d", count));
}

static void on_clear(void) {
  count = 0;
  sync_display();
  set_text("#status", "Cleared");
}

void counter_init(void) {
  sync_display();
  on_click("#btn", on_tap);
  on_click("#btn-reset", on_reset);
  on_click("#btn-double", on_double);
  on_click("#btn-ten", on_add_ten);
  on_click("#btn-clear", on_clear);
}
