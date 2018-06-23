#include <entry_list.h>

SDL_Surface *load_image(const char *image_path)
{
  SDL_Surface *temp = NULL;
  
  if((temp = IMG_Load(image_path)) != NULL)
  {
    return SDL_DisplayFormatAlpha(temp);
  }
  
  return NULL;
}

program_entry *get_directory_entrys(const char *directory_path, const char *default_icon_path, const char dir_entry_mode)
{
  program_entry *program_entry_list = NULL;
  program_entry *cache_program_entry = NULL;
  
  FILE *file_tester = NULL;
  DIR *program_dir = NULL;
  struct dirent *dir_pointer = NULL;
  char cache_path[cache_buffer_max];
  
  if((program_dir = opendir(directory_path)) != NULL)
  {
    /* check all directorys in program folder */
    while((dir_pointer = readdir(program_dir)) != NULL)
    {
      /* if dir_entry_package mode, search for 'install.sh' instead of same-named file */
      /* if dir_entry_theme mode, check '.' to proof that theme folder is a directory */
      if(dir_entry_mode == DIR_ENTRY_PACKAGE)
        sprintf(cache_path, "%s/%s/install.sh", directory_path, dir_pointer->d_name);
      else if(dir_entry_mode == DIR_ENTRY_THEME)
        sprintf(cache_path, "%s/%s/.", directory_path, dir_pointer->d_name);
      else
        sprintf(cache_path, "%s/%s/%s", directory_path, dir_pointer->d_name, dir_pointer->d_name);
      
      /* if folder is not '.' and contains file with same name OR is in dir_entry_theme mode */
      if(dir_pointer->d_name[0] != '.' && (file_tester = fopen(cache_path, "r")) != NULL)
      {
        fclose(file_tester);
        
        /* create or append new program_list item start */
        if(program_entry_list == NULL)
        {
          if((program_entry_list = malloc(sizeof(program_entry))) == NULL)
            return NULL;
          
          cache_program_entry = program_entry_list;
        }
        else
        {
          cache_program_entry = program_entry_list;
          
          /* search last entry */
          while(cache_program_entry->next != NULL)
          {
            cache_program_entry = cache_program_entry->next;
          }
          
          if((cache_program_entry->next = malloc(sizeof(program_entry))) == NULL)
            return NULL;
          
          cache_program_entry = cache_program_entry->next;
        }
        /* create or append new program_list item end */
        
        strcpy(cache_program_entry->name, dir_pointer->d_name);
        
        sprintf(cache_path, "%s/%s/%s.png", directory_path, dir_pointer->d_name, dir_pointer->d_name);
        
        /* check if image exist with same name as folder */
        if(dir_entry_mode == DIR_ENTRY_PROGRAM && (file_tester = fopen(cache_path, "r")) != NULL)
        {
          fclose(file_tester);
          
          if((cache_program_entry->big = load_image(cache_path)) != NULL)
          {
             cache_program_entry->small = shrinkSurface(cache_program_entry->big, 2, 2);
          }
          else
          {
            /* set default icons, if icon cant be loaded */
            if((cache_program_entry->big = load_image(default_icon_path)) == NULL)
              return NULL;
            cache_program_entry->small = shrinkSurface(cache_program_entry->big, 2, 2);
          }
        }
        else
        {
          /* set default icons, if icon not exist */
          if((cache_program_entry->big = load_image(default_icon_path)) == NULL)
            return NULL;
          cache_program_entry->small = shrinkSurface(cache_program_entry->big, 2, 2);
        }
        
        cache_program_entry->text_big = TTF_RenderText_Blended(get_font_big(), dir_pointer->d_name, get_font_color());
        cache_program_entry->text_small = TTF_RenderText_Blended(get_font_small(), dir_pointer->d_name, get_font_color());
        
        cache_program_entry->next = NULL;
      }
    }
    
    closedir(program_dir);
  }
  
  return program_entry_list;
}
void free_entry_list(program_entry *program_entry_list)
{
  program_entry *cache_program_entry = NULL;
  
  while(program_entry_list != NULL)
  {
    /* cache_program_entry points to the program entry to free */
    /* while program_entry_list points to the next program entry */
    cache_program_entry = program_entry_list;
    program_entry_list = program_entry_list->next;
    
    SDL_FreeSurface(cache_program_entry->big);
    SDL_FreeSurface(cache_program_entry->small);
    
    SDL_FreeSurface(cache_program_entry->text_big);
    SDL_FreeSurface(cache_program_entry->text_small);
    
    free(cache_program_entry);
  }
}

program_entry *get_command(const char *name, const char *command, const char *icon_path)
{
  program_entry *program_entry_list = NULL;
  
  /* append special commands */
  if((program_entry_list = malloc(sizeof(program_entry))) == NULL)
    return NULL;
  
  /* update interface */
  strcpy(program_entry_list->name, command);
  if((program_entry_list->big = load_image(icon_path)) == NULL)
    exit(EXIT_FAILURE);
  program_entry_list->small = shrinkSurface(program_entry_list->big, 2, 2);
  program_entry_list->text_big = TTF_RenderText_Blended(get_font_big(), name, get_font_color());
  program_entry_list->text_small = TTF_RenderText_Blended(get_font_small(), name, get_font_color());
  
  program_entry_list->next = NULL;
  
  return program_entry_list;
}
program_entry *get_interface_commands()
{
  program_entry *program_entry_list = NULL;
  program_entry *cache_program_entry = NULL;
  
  if((program_entry_list = get_command(COMMAND_PACKAGE_MANAGER, "//COMMAND_PACKAGE_MANAGER", icon_package_manager)) == NULL)
    return NULL;
  cache_program_entry = program_entry_list;
  
  if((cache_program_entry->next = get_command(COMMAND_THEME_MANAGER, "//COMMAND_THEME_MANAGER", icon_theme_manager)) == NULL)
    return NULL;
  cache_program_entry = cache_program_entry->next;
  
  if((cache_program_entry->next = get_command(COMMAND_REBOOT, "//COMMAND_REBOOT", icon_reboot)) == NULL)
    return NULL;
  cache_program_entry = cache_program_entry->next;
  
  if((cache_program_entry->next = get_command(COMMAND_SHUTDOWN, "//COMMAND_SHUTDOWN", icon_shutdown)) == NULL)
    return NULL;
  
  return program_entry_list;
}

void blit_entry_list(program_entry *program_entry_list, SDL_Surface *screen, const short current_program)
{
  program_entry *cache_program_entry = program_entry_list;
  short current_program_cache = 1;
  SDL_Rect cache_dest;
  
  if(current_program < max_displayable_programs/2)
    cache_dest.y = (Sint16)(max_displayable_programs/2 - current_program) * small_size;
  else
    cache_dest.y = 0;
  
  /* only draw visible programs and while not reached last program entry */
  while(cache_program_entry != NULL && cache_dest.y < screen->h)
  {
    if(current_program_cache > current_program - max_displayable_programs/2)
    {
      /* draw big icon/text if current selectet program */
      if(current_program_cache == current_program)
      {
        cache_dest.x = 0;
        SDL_BlitSurface(cache_program_entry->big, NULL, screen, &cache_dest);
        
        cache_dest.x += big_size;
        SDL_BlitSurface(cache_program_entry->text_big, NULL, screen, &cache_dest);
        
        cache_dest.y += big_size;
      }
      else
      {
        cache_dest.x = (Sint16)(big_size/2 - small_size/2);
        SDL_BlitSurface(cache_program_entry->small, NULL, screen, &cache_dest);
        
        cache_dest.x += small_size;
        SDL_BlitSurface(cache_program_entry->text_small, NULL, screen, &cache_dest);
        
        cache_dest.y += small_size;
      }
    }
    
    cache_program_entry = cache_program_entry->next;
    current_program_cache++;
  }
}
void animate_program_entry(program_entry *program_entry_list, SDL_Surface *screen, SDL_Surface *background, const short current_program, const char *text, const char animation_mode)
{
  program_entry *cache_program_entry = NULL;
  short current_program_cache;
  short animation_frame;
  Uint32 frame_timestop;
  
  SDL_Surface *text_surface = TTF_RenderText_Blended(get_font_small(), text, get_font_color());
  SDL_Rect cache_src;
  SDL_Rect cache_dest;
  
  if((animation_mode == ANIMATE_HIGHLIGHT_OUT) || (animation_mode == ANIMATE_HIGHLIGHT_IN_2))
    animation_frame = 9;
  else if(animation_mode == ANIMATE_HIGHLIGHT_OUT_2)
    animation_frame = 18;
  else if(animation_mode < ANIMATE_MOVE_OUT)
    animation_frame = 255;
  else
    animation_frame = 0;
  
  while(animation_frame >= 0 && animation_frame <= 255)
  {
    frame_timestop = SDL_GetTicks();
    
    SDL_BlitSurface(background, NULL, screen, NULL);
    
    /* animate not selectet program entrys till current_program */
    current_program_cache = 1;
    cache_program_entry = program_entry_list;
    
    cache_dest.y = (Sint16)(max_displayable_programs/2 - 1 - (current_program - current_program_cache)) * small_size - animation_frame * 1.5;
    while(current_program_cache < current_program)
    {
      /* blit only visible list entrys */
      if(cache_dest.y + small_size > 0)
      {
        /* draw first program_entry */
        if(cache_dest.y < 0)
        {
          cache_src.y = (0 - cache_dest.y);
          cache_src.x = 0;
          cache_src.w = small_size;
          cache_src.h = small_size - (0 - cache_dest.y);
          
          cache_dest.x = (Sint16)(big_size/2 - small_size/2);
          cache_dest.y = 0;
          SDL_BlitSurface(cache_program_entry->small, &cache_src, screen, &cache_dest);
          
          cache_dest.x += small_size;
          cache_src.w = cache_program_entry->text_small->w;
          SDL_BlitSurface(cache_program_entry->text_small, &cache_src, screen, &cache_dest);
          
          cache_dest.y = cache_src.h;
          cache_src.y = 0;
          cache_src.h = small_size;
          
          cache_program_entry = cache_program_entry->next;
          current_program_cache++;
          continue;
        }
        
        cache_dest.x = (Sint16)(big_size/2 - small_size/2);
        SDL_BlitSurface(cache_program_entry->small, NULL, screen, &cache_dest);
        
        cache_dest.x += small_size;
        SDL_BlitSurface(cache_program_entry->text_small, NULL, screen, &cache_dest);
      }
      cache_dest.y += small_size;
      
      cache_program_entry = cache_program_entry->next;
      current_program_cache++;
    }
    
    /* animate selectet program entry */
    cache_dest.x = 3 * animation_frame;
    
    cache_dest.y = (Sint16)(max_displayable_programs/2 - 1) * small_size;
    SDL_BlitSurface(cache_program_entry->big, NULL, screen, &cache_dest);
    
    cache_dest.x += big_size;
    SDL_BlitSurface(cache_program_entry->text_big, NULL, screen, &cache_dest);
    
    /* animate not selectet program entrys after current_program */
    cache_dest.y = (Sint16)(max_displayable_programs/2 + (current_program_cache - current_program) - 1) * small_size + big_size + animation_frame * 1.5;
    
    cache_program_entry = cache_program_entry->next;
    while(cache_program_entry != NULL && cache_dest.y < screen->h)
    {
      cache_dest.x = (Sint16)(big_size/2 - small_size/2);
      SDL_BlitSurface(cache_program_entry->small, NULL, screen, &cache_dest);
      
      cache_dest.x += small_size;
      SDL_BlitSurface(cache_program_entry->text_small, NULL, screen, &cache_dest);
      
      cache_dest.y += small_size;
      
      cache_program_entry = cache_program_entry->next;
    }
    
    /* draw box with text */
    cache_dest.x = 0;
    cache_dest.y = screen->h - small_size;
    draw_theme_box(screen, 0, screen->h - small_size, screen->w, small_size);
    SDL_BlitSurface(text_surface, NULL, screen, &cache_dest);
    
    /* fade out and keep frames */
    if((animation_mode == ANIMATE_MOVE_IN_FADE) || (animation_mode == ANIMATE_MOVE_OUT_FADE))
      boxRGBA(screen, 0, 0, screen->w, screen->h, 0, 0, 0, animation_frame);
    
    SDL_Flip(screen);
    if(15 > (SDL_GetTicks() - frame_timestop))
      SDL_Delay(15 - (SDL_GetTicks() - frame_timestop));
    
    if((animation_mode == ANIMATE_HIGHLIGHT_IN) || (animation_mode == ANIMATE_HIGHLIGHT_IN_2))
      animation_frame += 1;
    else if((animation_mode == ANIMATE_HIGHLIGHT_OUT) || (animation_mode == ANIMATE_HIGHLIGHT_OUT_2))
      animation_frame -= 1;
    else if(animation_mode < ANIMATE_MOVE_OUT)
      animation_frame -= 10;
    else
      animation_frame += 10;
    
    if(animation_mode == ANIMATE_HIGHLIGHT_IN && animation_frame > 9)
      animation_frame = -1;
    if(animation_mode == ANIMATE_HIGHLIGHT_IN_2 && animation_frame > 18)
      animation_frame = -1;
  }
  
  SDL_FreeSurface(text_surface);
}