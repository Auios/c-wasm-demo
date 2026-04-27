#include "range.h"
#include "aulib_wasm.h"

static void on_change(void) {
  int v = get_input_int("#demo-range");
  set_text("#range-value", au_format("%d", v));
  set_text("#status", au_format("Slider: %d", v));
}

void range_init(void) {
  set_text("#range-value", au_format("%d", get_input_int("#demo-range")));
  on_input("#demo-range", on_change);
}
