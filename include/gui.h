#pragma once

#include <libguile.h>

#include "utils.h"
#include "texture.h"

typedef enum gui_element_type {
	IMAGE,
	LABEL,
	DYNAMIC_LABEL,
	BUTTON,
	DIALOG_BOX,
	METER,
} gui_element_type;

typedef struct gui_element_data_image {
	texture *image;
} gui_element_data_image;

typedef struct gui_element_data_label {
	char text[256];
	double size;
} gui_element_data_label;

typedef struct gui_element_data_dynamic_label {
	SCM text_getter;
	double size;
} gui_element_data_dynamic_label;

typedef struct gui_element_data_button {
	char text[256];
	SCM click_callback;
} gui_element_data_button;

typedef struct gui_element_data_dialog_box {
	char text[1024];
} gui_element_data_dialog_box;

typedef struct gui_element_data_meter {
	char text[256];
	color c;
	SCM full_getter;
} gui_element_data_meter;

typedef union gui_element_data {
	gui_element_data_image image;
	gui_element_data_label label;
	gui_element_data_dynamic_label dynamic_label;
	gui_element_data_button button;
	gui_element_data_dialog_box dialog_box;
	gui_element_data_meter meter;
} gui_element_data;

typedef struct gui_element {
	gui_element_type type;
	gui_element_data data;
	double x, y;
	int w, h;
} gui_element;

void initialize_gui();

void reset_gui();

void load_bitmap_font(char *path);
int bitmap_index(char c);

void spawn_image(double x, double y, char *path);
void spawn_label(double x, double y, char *text, double size);
void spawn_dynamic_label(double x, double y, SCM getter, double size);
void spawn_button(double x, double y, char *text, SCM click_callback);
void spawn_dialog_box(double x, double y, char *text);
void spawn_meter(double x, double y, char *text, color c, SCM full_getter);

void mouse_clicked(double x, double y);

void render_text_bitmap(char *text, double x, double y, double size);

void draw_image(gui_element *ge);
void draw_label(gui_element *ge);
void draw_dynamic_label(gui_element *ge);
void draw_button(gui_element *ge);
void draw_dialog_box(gui_element *ge);
void draw_meter(gui_element *ge);

void draw_gui_element(gui_element *ge);

void draw_gui();
