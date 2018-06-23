#ifndef ENTRY_LIST_H
#define ENTRY_LIST_H

#include <dirent.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>

#include <font.h>

/* special commands */
#define COMMAND_PACKAGE_MANAGER "Paket Manager"
#define COMMAND_THEME_MANAGER "Theme Manager"
#define COMMAND_INSTALL "Installieren?"
#define MESSAGE_INSTALL "Installiere..."
#define COMMAND_DELETE "Entfernen?"
#define MESSAGE_DELETE "Entferne..."
#define COMMAND_QUIT "Beenden"
#define COMMAND_REBOOT "Neustarten"
#define COMMAND_SHUTDOWN "Herunterfahren"

#define INTERFACE_COMMANDS 4

#define name_max 128
#define cache_buffer_max 1024
#define interface_path "."
#define program_path "../programs"
#define package_path "../usb/pkg"
#define theme_path "../themes"

#define max_displayable_programs (short)((screen->h - big_size)/small_size + 2)

/* icons */
#define big_size 64
#define small_size 32
#define icon_default_program "default_program.png"
#define icon_default_package "default_package.png"
#define icon_default_theme "default_theme.png"
#define icon_package_manager "package_manager.png"
#define icon_theme_manager "theme_manager.png"
#define icon_reboot "reboot.png"
#define icon_shutdown "shutdown.png"

typedef struct program_entry{
  char name[name_max];
  
  SDL_Surface *big;
  SDL_Surface *small;
  
  SDL_Surface *text_big;
  SDL_Surface *text_small;
  
  struct program_entry *next;
}program_entry;

SDL_Surface *load_image(const char *image_path);

#define DIR_ENTRY_PROGRAM 0
#define DIR_ENTRY_PACKAGE 1
#define DIR_ENTRY_THEME 2
program_entry *get_directory_entrys(const char *directory_path, const char *default_icon_path, const char dir_entry_mode);
void free_entry_list(program_entry *program_entry_list);

program_entry *get_command(const char *name, const char *command, const char *icon_path);
program_entry *get_interface_commands();

void blit_entry_list(program_entry *program_entry_list, SDL_Surface *screen, const short current_program);

#define ANIMATE_MOVE_IN 1
#define ANIMATE_MOVE_IN_FADE 2
#define ANIMATE_MOVE_OUT 3
#define ANIMATE_MOVE_OUT_FADE 4
#define ANIMATE_HIGHLIGHT_IN 5
#define ANIMATE_HIGHLIGHT_IN_2 6
#define ANIMATE_HIGHLIGHT_OUT 7
#define ANIMATE_HIGHLIGHT_OUT_2 8
void animate_program_entry(program_entry *program_entry_list, SDL_Surface *screen, SDL_Surface *background, const short current_program, const char *text, const char animation_mode);

#endif