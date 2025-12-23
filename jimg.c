#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>


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
  SDL_Texture* texture = SDL_CreateTexture(renderer, pixelformat, SDL_TEXTUREACCESS_STATIC, x, y);
  SDL_UpdateTexture(texture, NULL, data, channels * x);


  float image_ratio = (float) x / (float) y;
  
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
      }
    }


    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);


    SDL_FRect rendering_rect;


    rendering_rect.w = window_width;
    rendering_rect.x = 0;
    rendering_rect.h = (float) rendering_rect.w / image_ratio;
    rendering_rect.y = (float) window_height / 2.0f - rendering_rect.h / 2.0f;

    SDL_RenderTexture(renderer, texture, NULL, &rendering_rect);


    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
} 

