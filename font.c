#include <font.h>

TTF_Font *font_big = NULL;
TTF_Font *font_small = NULL;
SDL_Color font_color = {0, 0, 0};
SDL_Color box_color = {0, 0, 0, 0};

int load_font()
{
  FILE *reader = NULL;
  char cache[color_buffer];
  
  int font_dpi_size = big_size;
  int pixel_size = 0;
  
  /* load font_big, reloads until font height is big_size */
  while(pixel_size != big_size)
  {
    /* load font with font_dpi_size */
    if((font_big = TTF_OpenFontIndex("font.ttf", font_dpi_size, 0)) == NULL)
      return -1;
    
    /* pixel_size equals the height in pixels from font_big opened with font_dpi_size */
    pixel_size = TTF_FontHeight(font_big);
    
    if(pixel_size < big_size)
    {
      TTF_CloseFont(font_big);
      font_dpi_size++;
    }
    else if(pixel_size > big_size)
    {
      TTF_CloseFont(font_big);
      font_dpi_size--;
    }
    
  }
  
  /* load font_small */
  font_dpi_size = small_size;
  pixel_size = 0;
  while(pixel_size != small_size)
  {
    if((font_small = TTF_OpenFontIndex("font.ttf", font_dpi_size, 0)) == NULL)
      return -1;
    
    pixel_size = TTF_FontHeight(font_small);
    
    if(pixel_size < small_size)
    {
      TTF_CloseFont(font_small);
      font_dpi_size++;
    }
    else if(pixel_size > small_size)
    {
      TTF_CloseFont(font_small);
      font_dpi_size--;
    }
    
  }
  
  if((reader = fopen("font_color", "r")) == NULL)
    return -1;
  
  if(fgets(cache, color_buffer, reader) == NULL)
    return -1;
  font_color.r = atoi(cache);
  
  if(fgets(cache, color_buffer, reader) == NULL)
    return -1;
  font_color.g = atoi(cache);
  
  if(fgets(cache, color_buffer, reader) == NULL)
    return -1;
  font_color.b = atoi(cache);
  
  if(fgets(cache, color_buffer, reader) == NULL)
    return -1;
  box_color.r = atoi(cache);
  
  if(fgets(cache, color_buffer, reader) == NULL)
    return -1;
  box_color.g = atoi(cache);
  
  if(fgets(cache, color_buffer, reader) == NULL)
    return -1;
  box_color.b = atoi(cache);
  
  if(fgets(cache, color_buffer, reader) == NULL)
    return -1;
  box_color.unused = atoi(cache);
  
  fclose(reader);
  return 0;
}
void unload_font()
{
  TTF_CloseFont(font_big);
  TTF_CloseFont(font_small);
}

TTF_Font *get_font_big()
{
  return font_big;
}
TTF_Font *get_font_small()
{
  return font_small;
}
SDL_Color get_font_color()
{
  return font_color;
}
int get_font_small_text_w(const char *text)
{
  int w;
  
  TTF_SizeText(font_small, text, &w, NULL);
  return w;
}

void draw_theme_box(SDL_Surface *screen, Sint16 x, Sint16 y, Sint16 w, Sint16 h)
{
  boxRGBA(screen, x, y, x + w - 1, y + h - 1, box_color.r, box_color.g, box_color.b, box_color.unused);
}