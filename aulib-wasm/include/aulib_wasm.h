#pragma once

__attribute__((import_module("env"), import_name("set_text"))) void
set_text(const char *selector, const char *text);

__attribute__((import_module("env"), import_name("on_click"))) void
on_click(const char *selector, void (*handler)(void));

__attribute__((import_module("env"), import_name("on_input"))) void
on_input(const char *selector, void (*handler)(void));

__attribute__((import_module("env"), import_name("get_input_int"))) int
get_input_int(const char *selector);

__attribute__((import_module("env"), import_name("on_key"))) void
on_key(void (*handler)(int code));

__attribute__((import_module("env"), import_name("request_frame"))) void
request_frame(void (*handler)(void));

__attribute__((import_module("env"), import_name("rand_below"))) int
rand_below(int n);

__attribute__((import_module("env"), import_name("canvas_width"))) int
canvas_width(const char *selector);

__attribute__((import_module("env"), import_name("canvas_height"))) int
canvas_height(const char *selector);

__attribute__((import_module("env"), import_name("canvas_clear"))) void
canvas_clear(const char *selector, unsigned int color);

__attribute__((import_module("env"), import_name("canvas_fill_rect"))) void
canvas_fill_rect(const char *selector, int x, int y, int w, int h,
                 unsigned int color);

__attribute__((import_module("env"), import_name("canvas_fill_circle"))) void
canvas_fill_circle(const char *selector, int x, int y, int r,
                   unsigned int color);

char *au_format(const char *fmt, ...);
