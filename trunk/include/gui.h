#ifndef PURGE_GUI_H
#define PURGE_GUI_H

void initialize_gui();
void load_bitmap_font(char *path);
int bitmap_index(char c);
void render_text_bitmap(int x, int y, char *text, double size);
void update_gui();
void draw_button(char *text, int x, int y);
void draw_dialog_box(char *text, int x, int y);
void draw_gui();
#endif
