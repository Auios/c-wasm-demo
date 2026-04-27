#include "wasm.h"
#include <stdarg.h>

int count = 0;

#define FORMAT_CAP 256
static char format_buf[FORMAT_CAP];

static char *format(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char *d = format_buf;
  const char *end = format_buf + sizeof(format_buf) - 1U;

  while (*fmt && d < end) {
    if (fmt[0] != '%') {
      *d++ = *fmt++;
      continue;
    }
    fmt++;
    if (*fmt == '\0') {
      break;
    }
    if (*fmt == '%') {
      *d++ = '%';
      fmt++;
    } else if (*fmt == 'd') {
      int n = va_arg(ap, int);
      unsigned u;
      if (n < 0) {
        if (d < end) {
          *d++ = '-';
        }
        u = (unsigned)(-(n + 1)) + 1U;
      } else {
        u = (unsigned)n;
      }
      char tmp[10];
      int w = 0;
      if (u == 0U) {
        tmp[w++] = '0';
      } else {
        while (u > 0U) {
          tmp[w++] = (char)('0' + (u % 10U));
          u /= 10U;
        }
      }
      while (w > 0 && d < end) {
        *d++ = tmp[--w];
      }
      fmt++;
    } else {
      *d++ = '%';
      *d++ = *fmt++;
    }
  }
  *d = '\0';
  va_end(ap);
  return format_buf;
}

static void handle_click(void) {
  count++;
  set_text("#message", format("Clicked %d times", count));
}

__attribute__((export_name("init"))) void init(void) {
  set_text("#message", "Hello world from C");
  on_click("#btn", handle_click);
}
