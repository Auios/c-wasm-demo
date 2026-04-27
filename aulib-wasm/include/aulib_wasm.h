#pragma once

__attribute__((import_module("env"), import_name("set_text"))) void
set_text(const char *selector, const char *text);

__attribute__((import_module("env"), import_name("on_click"))) void
on_click(const char *selector, void (*handler)(void));

__attribute__((import_module("env"), import_name("on_input"))) void
on_input(const char *selector, void (*handler)(void));

__attribute__((import_module("env"), import_name("get_input_int"))) int
get_input_int(const char *selector);

char *au_format(const char *fmt, ...);
