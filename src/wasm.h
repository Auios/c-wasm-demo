#pragma once

__attribute__((import_module("env"), import_name("set_text"))) void
set_text(const char *selector, const char *text);

__attribute__((import_module("env"), import_name("on_click"))) void
on_click(const char *selector, void (*handler)(void));
