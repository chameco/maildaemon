#pragma once

#include "utils.h"

void initialize_gui();

void load_bitmap_font(char *path);
int bitmap_index(char c);

void update_gui();

void render_text_bitmap(char *text, int x, int y, double size);
void draw_button(char *text, int x, int y);
void draw_dialog_box(char *text, int x, int y);
void draw_meter(char *text, int x, int y, color c, int full);
void draw_gui();
