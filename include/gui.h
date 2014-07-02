#pragma once

#include "utils.h"

void initialize_gui();

void load_bitmap_font(char *path);
int bitmap_index(char c);

void render_text_bitmap(char *text, double x, double y, double size);
void draw_button(char *text, double x, double y);
void draw_dialog_box(char *text, double x, double y);
void draw_meter(char *text, double x, double y, color c, double full);
