#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

// disable unwanted file formats for smaller stbi footprint
#define STBI_NO_PSD
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_TGA
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"



int main(int argc, char** argv) {

  if(argc != 2) {
    printf("Specify image to open with jimg. Usage: jimg image.png\n");
    return -1;
  }

  char* filename = argv[1];


  int x,y,channels;

  // An output image with N components has the following components interleaved
// in this order in each pixel:
//
//     N=#comp     components
//       1           grey
//       2           grey, alpha
//       3           red, green, blue
//       4           red, green, blue, alpha
//
  unsigned char* data = stbi_load(filename, &x, &y, &channels, 0);

  if(data == NULL) {
    printf("Could not read file.\n");
    return -1;
  }

  if(!SDL_Init(SDL_INIT_VIDEO)) {
    printf("SDL could not be initialised!\n");
    return -1;
  }

  SDL_Window* window = SDL_CreateWindow("JIMG", 1280, 720, SDL_WINDOW_RESIZABLE);
  int window_width = 1280;
  int window_height = 720;

  if(window == NULL) {
    printf("Window could not be created.\n");
    SDL_Quit();
    return -1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
  if(renderer == NULL) {
    printf("SDL Renderer could not be created!\n");
    SDL_Quit();
    return -1;
  }

  SDL_PixelFormat pixelformat = SDL_PIXELFORMAT_UNKNOWN;
  if(channels == 3) pixelformat = SDL_PIXELFORMAT_RGB24;
  if(channels == 4) pixelformat = SDL_PIXELFORMAT_ARGB8888;

  if(pixelformat == SDL_PIXELFORMAT_UNKNOWN) {
    printf("Pixelformat could not be interpreted.\n");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  SDL_Cursor* cursor_default = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
  SDL_Cursor* cursor_movement = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);

  if(cursor_movement == 0 || cursor_default == 0) {
    printf("Cursors could not be created\n");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }


  SDL_Texture* texture = SDL_CreateTexture(renderer, pixelformat, SDL_TEXTUREACCESS_STATIC, x, y);
  SDL_UpdateTexture(texture, NULL, data, channels * x);

  float image_ratio = (float) x / (float) y;
  float scale_factor = 1.0f;
  int capture_movement = 0;
  float xoffset = 0.0f;
  float yoffset = 0.0f;

  int shouldQuit = 0;

  SDL_Event event;
  while(!shouldQuit) {
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_EVENT_QUIT:
          shouldQuit = 1;
          break;
        case SDL_EVENT_WINDOW_RESIZED:
          int w = event.window.data1;
          int h = event.window.data2;
          window_width = w;
          window_height = h;
          break;
        case SDL_EVENT_MOUSE_WHEEL:
          if(event.wheel.y > 0) {
            // zoom in
            scale_factor = scale_factor * (1.08f * event.wheel.y);
            printf("mouse wheel %f\n", event.wheel.y);
          } else if(event.wheel.y < 0) {
            // zoom out
            scale_factor = scale_factor / (-1.08f * event.wheel.y);
          }
          break;
        case SDL_EVENT_KEY_DOWN:
          // reset all scrolling and panning
          if(event.key.key == SDLK_SPACE) {
            scale_factor = 1;
            xoffset = 0.0f;
            yoffset = 0.0f;
          }
          break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
          if(event.button.button == SDL_BUTTON_LEFT) {
            capture_movement = 1;
            SDL_SetCursor(cursor_movement);
          }
          break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
          if(event.button.button == SDL_BUTTON_LEFT) {
            capture_movement = 0;
            SDL_SetCursor(cursor_default);
          }
          break;
        case SDL_EVENT_MOUSE_MOTION:
          if(capture_movement) {
            xoffset += event.motion.xrel;
            yoffset += event.motion.yrel;
          }
          break;
      }
    }


    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);


    SDL_FRect rendering_rect;


    rendering_rect.w = window_width * scale_factor;
    rendering_rect.x = (window_width / 2.0f) - (rendering_rect.w / 2.0f) + xoffset;
    rendering_rect.h = (float) rendering_rect.w / image_ratio;
    rendering_rect.y = (float) (window_height) / 2.0f - rendering_rect.h / 2.0f + yoffset;

    SDL_RenderTexture(renderer, texture, NULL, &rendering_rect);


    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
} 

