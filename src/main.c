#include "aulib_wasm.h"

int count = 0;

static void handle_click(void) {
  count++;
  set_text("#message", au_format("Clicked %d times", count));
}

__attribute__((export_name("init"))) void init(void) {
  set_text("#message", "Hello world from C");
  on_click("#btn", handle_click);
}
