#ifndef FONT_H
#define FONT_H

#include <SDL.h>
#include <SDL_ttf.h>

#include <entry_list.h>

#define color_buffer 6

int load_font();
void unload_font();

TTF_Font *get_font_big();
TTF_Font *get_font_small();
SDL_Color get_font_color();
int get_font_small_text_w(const char *text);

void draw_theme_box(SDL_Surface *screen, Sint16 x, Sint16 y, Sint16 w, Sint16 h);

#endif