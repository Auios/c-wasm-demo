#include "aulib_wasm.h"
#include <stdarg.h>

#define AU_FORMAT_CAP 256
static char au_format_buf[AU_FORMAT_CAP];

char *au_format(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char *d = au_format_buf;
  const char *end = au_format_buf + sizeof(au_format_buf) - 1U;

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
  return au_format_buf;
}
