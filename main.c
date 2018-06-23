#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <SDL.h>
#include <SDL_mixer.h>

#include <entry_list.h>
#include <font.h>

/* return values of main() */
#define PROGRAM_RUNNING 1
#define PROGRAM_RESTART_INTERFACE 2
#define PROGRAM_REBOOT 3
#define PROGRAM_SHUTDOWN 4

#define MODE_DEFAULT 0
#define MODE_PACKAGE_MANAGER 1
#define MODE_THEME_MANAGER 2

#define supported_joysticks 4

int main()
{
  /* variables */
  char program_run = PROGRAM_RUNNING;
  char interface_mode = MODE_DEFAULT;
  
  SDL_Surface *screen = NULL;
  SDL_Surface *background = NULL;
  SDL_Event user_input;
  Uint32 frame_timestop;
  
  /* gamepad/joystick */
  SDL_Joystick *joystick[supported_joysticks];
  
  int joystick_cache_counter;
  int max_joysticks;
  
  /* program entrys */
  short program_max = 0;
  
  short current_program = 1;
  short current_program_cache = 1;
  SDL_Rect cache_dest;
  
  program_entry *program_entry_list = NULL;
  program_entry *cache_program_entry = NULL;
  
  char cache_path[1024];
  
  /* set locales */
  setlocale(LC_TIME, "");
  SDL_Surface *clock_surface = NULL;
  time_t src_time;
  
  /* sounds */
  Mix_Music *choose_sound = NULL;
  Mix_Music *select_sound = NULL;
  Mix_Music *delete_sound = NULL;
  
  /* init SDL start */
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    exit(EXIT_FAILURE);
  
  if((screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE)) == NULL)
    exit(EXIT_FAILURE);
  
  atexit(SDL_Quit);
  SDL_ShowCursor(SDL_DISABLE);
  SDL_EnableKeyRepeat(200, 50);
  /* init SDL end */
  
  /* init joysticks */
  max_joysticks = SDL_NumJoysticks();
  if(max_joysticks > supported_joysticks)
    max_joysticks = supported_joysticks;
  
  joystick_cache_counter = 0;
  while(joystick_cache_counter < max_joysticks)
  {
    joystick[joystick_cache_counter] = SDL_JoystickOpen(joystick_cache_counter);
    joystick_cache_counter++;
  }
  
  SDL_JoystickEventState(SDL_ENABLE);
  
  /* init music start */
  if(Mix_OpenAudio(22050, AUDIO_S16, 2, 4096) < 0)
    exit(EXIT_FAILURE);
  atexit(Mix_CloseAudio);
  
  if((choose_sound = Mix_LoadMUS("choose.wav")) == NULL)
    exit(EXIT_FAILURE);
  
  if((select_sound = Mix_LoadMUS("select.wav")) == NULL)
    exit(EXIT_FAILURE);
  
  if((delete_sound = Mix_LoadMUS("delete.wav")) == NULL)
    exit(EXIT_FAILURE);
  /* init music end */
  
  /* init SDL_ttf */
  if(TTF_Init() < 0)
    exit(EXIT_FAILURE);
  if(load_font() < 0)
    exit(EXIT_FAILURE);
  atexit(unload_font);
  
  if((background = load_image("background.png")) == NULL)
    exit(EXIT_FAILURE);
  
  /* load program entrys */
  program_entry_list = get_directory_entrys(program_path, icon_default_program, DIR_ENTRY_PROGRAM);
  
  if(program_entry_list == NULL)
  {
    if((program_entry_list = get_interface_commands()) == NULL)
      exit(EXIT_FAILURE);
  }
  else
  {
    cache_program_entry = program_entry_list;
    program_max++;
    
    /* search last entry */
    while(cache_program_entry->next != NULL)
    {
      cache_program_entry = cache_program_entry->next;
      program_max++;
    }
    
    if((cache_program_entry->next = get_interface_commands()) == NULL)
      exit(EXIT_FAILURE);
  }
  
  program_max += INTERFACE_COMMANDS;
  /* add special program commands end */
  
  /* main loop start */
  /* only if one program is loaded at least */
  while(program_run == PROGRAM_RUNNING)
  {
    frame_timestop = SDL_GetTicks();
    
    /* handle user input event */
    while(SDL_PollEvent(&user_input))
    {
      if(current_program > 1 && ((user_input.type == SDL_KEYDOWN && user_input.key.keysym.sym == SDLK_UP) ||
        (user_input.type == SDL_JOYAXISMOTION && user_input.jaxis.axis == 1 && user_input.jaxis.value < 0)))
      {
        Mix_PlayMusic(choose_sound, 0);
        current_program--;
      }
      else if(current_program < program_max && ((user_input.type == SDL_KEYDOWN && user_input.key.keysym.sym == SDLK_DOWN) ||
        (user_input.type == SDL_JOYAXISMOTION && user_input.jaxis.axis == 1 && user_input.jaxis.value > 0)))
      {
        Mix_PlayMusic(choose_sound, 0);
        current_program++;
      }
      else if(((user_input.type == SDL_KEYDOWN && user_input.key.keysym.sym == SDLK_SPACE) ||
        (user_input.type == SDL_JOYBUTTONDOWN && user_input.jbutton.button == 2)))
      {
        current_program_cache = 1;
        cache_program_entry = program_entry_list;
        
        /* find selectet program_entry */
        while(current_program_cache < current_program)
        {
          cache_program_entry = cache_program_entry->next;
          current_program_cache++;
        }
        
        if(interface_mode == MODE_PACKAGE_MANAGER)
        {
          if(strcmp(cache_program_entry->name, "//COMMAND_QUIT") == 0)
          {
            Mix_PlayMusic(select_sound, 0);
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_OUT);
            interface_mode = MODE_DEFAULT;
            
            /* system("umount /usb"); */
            
            free_entry_list(program_entry_list);
            program_max = 0;
            
            program_entry_list = get_directory_entrys(program_path, icon_default_program, DIR_ENTRY_PROGRAM);
            
            if(program_entry_list == NULL)
            {
              if((program_entry_list = get_interface_commands()) == NULL)
                exit(EXIT_FAILURE);
            }
            else
            {
              cache_program_entry = program_entry_list;
              program_max++;
              
              /* search last entry */
              while(cache_program_entry->next != NULL)
              {
                cache_program_entry = cache_program_entry->next;
                program_max++;
              }
              
              if((cache_program_entry->next = get_interface_commands()) == NULL)
                exit(EXIT_FAILURE);
            }
            
            program_max += INTERFACE_COMMANDS;
            current_program = program_max - INTERFACE_COMMANDS + 1;
            
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_IN);
          }
          else
          {
            animate_program_entry(program_entry_list, screen, background, current_program, COMMAND_INSTALL, ANIMATE_HIGHLIGHT_IN);
            
            do
            {
              SDL_WaitEvent(&user_input);
              /* while select/abbort not pressed */
            }while(!((user_input.type == SDL_KEYDOWN) || (user_input.type == SDL_JOYBUTTONDOWN)));
            
            if((user_input.type == SDL_KEYDOWN && user_input.key.keysym.sym == SDLK_SPACE) ||
                    (user_input.type == SDL_JOYBUTTONDOWN && user_input.jbutton.button == 2))
            {
              Mix_PlayMusic(select_sound, 0);
              
              animate_program_entry(program_entry_list, screen, background, current_program, MESSAGE_INSTALL, ANIMATE_HIGHLIGHT_IN_2);
              
              sprintf(cache_path, "cd \"%s/%s\" && bash \"install.sh\"", package_path, cache_program_entry->name);
              system(cache_path);
              
              animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_HIGHLIGHT_OUT_2);
            }
            else
            {
              Mix_PlayMusic(choose_sound, 0);
              animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_HIGHLIGHT_OUT);
            }
          }
          
          break;
        }
        else if(interface_mode == MODE_THEME_MANAGER)
        {
          Mix_PlayMusic(select_sound, 0);
          
          if(strcmp(cache_program_entry->name, "//COMMAND_QUIT") == 0)
          {
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_OUT);
            interface_mode = MODE_DEFAULT;
            
            free_entry_list(program_entry_list);
            program_max = 0;
            
            program_entry_list = get_directory_entrys(program_path, icon_default_program, DIR_ENTRY_PROGRAM);
            
            if(program_entry_list == NULL)
            {
              if((program_entry_list = get_interface_commands()) == NULL)
                exit(EXIT_FAILURE);
            }
            else
            {
              cache_program_entry = program_entry_list;
              program_max++;
              
              /* search last entry */
              while(cache_program_entry->next != NULL)
              {
                cache_program_entry = cache_program_entry->next;
                program_max++;
              }
              
              if((cache_program_entry->next = get_interface_commands()) == NULL)
                exit(EXIT_FAILURE);
            }
            
            program_max += INTERFACE_COMMANDS;
            current_program = program_max - INTERFACE_COMMANDS + 2;
            
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_IN);
          }
          else
          {
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_OUT_FADE);
            
            sprintf(cache_path, "cp -rf \"%s/%s/\"* \"%s/\"", theme_path, cache_program_entry->name, interface_path);
            system(cache_path);
            
            free_entry_list(program_entry_list);
            
            /* reload graphics/sounds/etc. */
            Mix_FreeMusic(choose_sound);
            Mix_FreeMusic(select_sound);
            Mix_FreeMusic(delete_sound);
            
            unload_font();
            
            SDL_FreeSurface(background);
            
            if((choose_sound = Mix_LoadMUS("choose.wav")) == NULL)
              exit(EXIT_FAILURE);
            
            if((select_sound = Mix_LoadMUS("select.wav")) == NULL)
              exit(EXIT_FAILURE);
            
            if((delete_sound = Mix_LoadMUS("delete.wav")) == NULL)
              exit(EXIT_FAILURE);
            
            if(load_font() < 0)
              exit(EXIT_FAILURE);
            
            if((background = load_image("background.png")) == NULL)
              exit(EXIT_FAILURE);
            
            /* reload interface */
            program_entry_list = get_directory_entrys(theme_path, icon_default_theme, DIR_ENTRY_THEME);
            
            if(program_entry_list == NULL)
            {
              if((program_entry_list = get_command(COMMAND_QUIT, "//COMMAND_QUIT", "quit.png")) == NULL)
                exit(EXIT_FAILURE);
            }
            else
            {
              cache_program_entry = program_entry_list;
              
              /* search last entry */
              while(cache_program_entry->next != NULL)
              {
                cache_program_entry = cache_program_entry->next;
              }
              
              if((cache_program_entry->next = get_command(COMMAND_QUIT, "//COMMAND_QUIT", "quit.png")) == NULL)
                exit(EXIT_FAILURE);
            }
            
            /* move in reloaded interface */
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_IN_FADE);
          }
          
          break;
        }
        else
        {
          Mix_PlayMusic(select_sound, 0);
          
          if(strcmp(cache_program_entry->name, "//COMMAND_PACKAGE_MANAGER") == 0)
          {
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_OUT);
            interface_mode = MODE_PACKAGE_MANAGER;
            
            /* system("mount /dev/sdb1 /usb"); */
            
            free_entry_list(program_entry_list);
            program_max = 0;
            current_program = 1;
            
            program_entry_list = get_directory_entrys(package_path, icon_default_package, DIR_ENTRY_PACKAGE);
            
            if(program_entry_list == NULL)
            {
              if((program_entry_list = get_command(COMMAND_QUIT, "//COMMAND_QUIT", "quit.png")) == NULL)
                exit(EXIT_FAILURE);
            }
            else
            {
              cache_program_entry = program_entry_list;
              program_max++;
              
              /* search last entry */
              while(cache_program_entry->next != NULL)
              {
                cache_program_entry = cache_program_entry->next;
                program_max++;
              }
              
              if((cache_program_entry->next = get_command(COMMAND_QUIT, "//COMMAND_QUIT", "quit.png")) == NULL)
                exit(EXIT_FAILURE);
            }
            
            program_max++;
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_IN);
          }
          else if(strcmp(cache_program_entry->name, "//COMMAND_THEME_MANAGER") == 0)
          {
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_OUT);
            interface_mode = MODE_THEME_MANAGER;
            
            free_entry_list(program_entry_list);
            program_max = 0;
            current_program = 1;
            
            program_entry_list = get_directory_entrys(theme_path, icon_default_theme, DIR_ENTRY_THEME);
            
            if(program_entry_list == NULL)
            {
              if((program_entry_list = get_command(COMMAND_QUIT, "//COMMAND_QUIT", "quit.png")) == NULL)
                exit(EXIT_FAILURE);
            }
            else
            {
              cache_program_entry = program_entry_list;
              program_max++;
              
              /* search last entry */
              while(cache_program_entry->next != NULL)
              {
                cache_program_entry = cache_program_entry->next;
                program_max++;
              }
              
              if((cache_program_entry->next = get_command(COMMAND_QUIT, "//COMMAND_QUIT", "quit.png")) == NULL)
                exit(EXIT_FAILURE);
            }
            
            program_max++;
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_IN);
          }
          else if(strcmp(cache_program_entry->name, "//COMMAND_REBOOT") == 0)
          {
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_OUT_FADE);
            program_run = PROGRAM_REBOOT;
          }
          else if(strcmp(cache_program_entry->name, "//COMMAND_SHUTDOWN") == 0)
          {
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_OUT_FADE);
            program_run = PROGRAM_SHUTDOWN;
          }
          else
          {
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_OUT_FADE);
            sprintf(cache_path, "cd \"%s/%s\" && xinit \"%s/%s/%s\" -- :1", program_path, cache_program_entry->name, program_path, cache_program_entry->name, cache_program_entry->name);
            /* system(cache_path); */
          }
          
          break;
        }
      }
      else if(((user_input.type == SDL_KEYDOWN && user_input.key.keysym.sym == SDLK_r) ||
        (user_input.type == SDL_JOYBUTTONDOWN && user_input.jbutton.button == 0)))
      {
        current_program_cache = 1;
        cache_program_entry = program_entry_list;
        
        /* find selectet program_entry */
        while(current_program_cache < current_program)
        {
          cache_program_entry = cache_program_entry->next;
          current_program_cache++;
        }
        
        /* delete mode */
        if(interface_mode == MODE_DEFAULT && current_program <= program_max - INTERFACE_COMMANDS)
        {
          Mix_PlayMusic(choose_sound, 0);
          animate_program_entry(program_entry_list, screen, background, current_program, COMMAND_DELETE, ANIMATE_HIGHLIGHT_IN);
          
          do
          {
            SDL_WaitEvent(&user_input);
            /* while select/abbort not pressed */
          }while(!((user_input.type == SDL_KEYDOWN) || (user_input.type == SDL_JOYBUTTONDOWN)));
          
          if((user_input.type == SDL_KEYDOWN && user_input.key.keysym.sym == SDLK_SPACE) ||
                  (user_input.type == SDL_JOYBUTTONDOWN && user_input.jbutton.button == 2))
          {
            animate_program_entry(program_entry_list, screen, background, current_program, MESSAGE_DELETE, ANIMATE_HIGHLIGHT_OUT);
            Mix_PlayMusic(delete_sound, 0);
            animate_program_entry(program_entry_list, screen, background, current_program, MESSAGE_DELETE, ANIMATE_MOVE_OUT);
            
            sprintf(cache_path, "rm -rf \"%s/%s\"", program_path, cache_program_entry->name);
            system(cache_path);
            
            /* reload interface */
            free_entry_list(program_entry_list);
            program_entry_list = get_directory_entrys(program_path, icon_default_program, DIR_ENTRY_PROGRAM);
            program_max = 0;
            
            if(program_entry_list == NULL)
            {
              if((program_entry_list = get_interface_commands()) == NULL)
                exit(EXIT_FAILURE);
            }
            else
            {
              cache_program_entry = program_entry_list;
              program_max++;
              
              /* search last entry */
              while(cache_program_entry->next != NULL)
              {
                cache_program_entry = cache_program_entry->next;
                program_max++;
              }
              
              if((cache_program_entry->next = get_interface_commands()) == NULL)
                exit(EXIT_FAILURE);
            }
            
            program_max += INTERFACE_COMMANDS;
            if(current_program > 1)
              current_program--;
            
            /* move in reloaded interface */
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_IN);
          }
          else
          {
            Mix_PlayMusic(choose_sound, 0);
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_HIGHLIGHT_OUT);
          }
        }
        else if(interface_mode == MODE_THEME_MANAGER && current_program != program_max)
        {
          Mix_PlayMusic(choose_sound, 0);
          animate_program_entry(program_entry_list, screen, background, current_program, COMMAND_DELETE, ANIMATE_HIGHLIGHT_IN);
          
          do
          {
            SDL_WaitEvent(&user_input);
            /* while select/abbort not pressed */
          }while(!((user_input.type == SDL_KEYDOWN) || (user_input.type == SDL_JOYBUTTONDOWN)));
          
          if((user_input.type == SDL_KEYDOWN && user_input.key.keysym.sym == SDLK_SPACE) ||
                  (user_input.type == SDL_JOYBUTTONDOWN && user_input.jbutton.button == 2))
          {
            animate_program_entry(program_entry_list, screen, background, current_program, MESSAGE_DELETE, ANIMATE_HIGHLIGHT_OUT);
            Mix_PlayMusic(delete_sound, 0);
            animate_program_entry(program_entry_list, screen, background, current_program, MESSAGE_DELETE, ANIMATE_MOVE_OUT);
            
            sprintf(cache_path, "rm -rf \"%s/%s\"", theme_path, cache_program_entry->name);
            system(cache_path);
            
            /* reload interface */
            free_entry_list(program_entry_list);
            program_entry_list = get_directory_entrys(theme_path, icon_default_theme, DIR_ENTRY_THEME);
            program_max = 0;
            
            if(program_entry_list == NULL)
            {
              if((program_entry_list = get_command(COMMAND_QUIT, "//COMMAND_QUIT", "quit.png")) == NULL)
                exit(EXIT_FAILURE);
            }
            else
            {
              cache_program_entry = program_entry_list;
              program_max++;
              
              /* search last entry */
              while(cache_program_entry->next != NULL)
              {
                cache_program_entry = cache_program_entry->next;
                program_max++;
              }
              
              if((cache_program_entry->next = get_command(COMMAND_QUIT, "//COMMAND_QUIT", "quit.png")) == NULL)
                exit(EXIT_FAILURE);
            }
            
            program_max++;
            if(current_program > 1)
              current_program--;
            
            /* move in reloaded interface */
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_MOVE_IN);
          }
          else
          {
            Mix_PlayMusic(choose_sound, 0);
            animate_program_entry(program_entry_list, screen, background, current_program, "", ANIMATE_HIGHLIGHT_OUT);
          }
        }
        
        break;
      }
    }
    
    /* re-draw screen */
    SDL_BlitSurface(background, NULL, screen, NULL);
    
    blit_entry_list(program_entry_list, screen, current_program);
    
    /* draw clock, uses cache_path as cache string */
    time(&src_time);
    
    strftime(cache_path, cache_buffer_max, "%A, %d. %B", localtime(&src_time));
    clock_surface = TTF_RenderText_Blended(get_font_small(), cache_path, get_font_color());
    cache_dest.x = 0;
    cache_dest.y = screen->h - small_size;
    
    draw_theme_box(screen, 0, screen->h - small_size, screen->w, small_size);
    SDL_BlitSurface(clock_surface, NULL, screen, &cache_dest);
    SDL_FreeSurface(clock_surface);
    
    strftime(cache_path, cache_buffer_max, "%H:%M:%S", localtime(&src_time));
    clock_surface = TTF_RenderText_Blended(get_font_small(), cache_path, get_font_color());
    cache_dest.x = screen->w - get_font_small_text_w(cache_path);
    SDL_BlitSurface(clock_surface, NULL, screen, &cache_dest);
    SDL_FreeSurface(clock_surface);
    
    /* for graphical purposes */
    if(program_run == PROGRAM_RUNNING)
      SDL_Flip(screen);
    
    /* keep framerate */
    if(15 > (SDL_GetTicks() - frame_timestop))
      SDL_Delay(15 - (SDL_GetTicks() - frame_timestop));
  }
  /* main loop end */
  /* free joysticks */
  joystick_cache_counter = 0;
  while(joystick_cache_counter < max_joysticks)
  {
    SDL_JoystickClose(joystick[joystick_cache_counter]);
    joystick_cache_counter++;
  }
  
  free_entry_list(program_entry_list);
  
  /* free sounds */
  Mix_FreeMusic(choose_sound);
  Mix_FreeMusic(select_sound);
  Mix_FreeMusic(delete_sound);
  
  /* free surfaces */
  SDL_FreeSurface(background);
  
  return program_run;
}