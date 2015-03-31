#include <stdint.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include "sdl_helper.h"

#include "composer.h"
#include "bitmap.h"

SDL_Surface *screen;
layer_t *scene;
layer_t *frame;
layer_t *icon;

void setup() {
  scene = layer_new();
  layer_init_size(scene, 640, 480);
  layer_fill(scene, 0x00FFFFFF);

  frame = bitmap_new("res/frame.png");
  icon = bitmap_new("res/icon.png");
  
  bitmap_resize(icon, 64, 64);
  
  add_child(scene, frame);
  add_child(frame, icon);
  layer_move(frame, 0, 16, true);
}

bool loop() {
  SDL_Event e;

  layer_move(icon, 1, 0, true);
  layer_move(frame, 1, 0, true);

  Sleep(10);
  compose(scene);

  SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(scene->composite, scene->composite_w, scene->composite_h, 32, scene->composite_w * 4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
  if(SDL_MUSTLOCK(screen)) if(SDL_LockSurface(screen) < 0) exit(1);
  SDL_FillRect(screen, NULL, 0xFFFFFF00);
  SDL_BlitSurface(surface, NULL, screen, NULL);
  SDL_FreeSurface(surface);
  if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  SDL_Flip(screen); 
  
  if(SDL_PollEvent(&e)) {
    switch(e.type) {
      case SDL_KEYDOWN:
        switch(e.key.keysym.sym) {
          case SDLK_ESCAPE: return false;
        }
        break;
    }
  }
  return true;
}


int main(int argc, char** argv) {
  if(SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
  if(screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE)) {
    setup();
    while(loop());
  }

  SDL_Quit();
  return 0;
}