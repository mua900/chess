#include "application.h"

bool create_application(Application* app)
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    return false;
  }
  
  int init_width = 1440;
  int init_height = 810;
  int flags = SDL_WINDOW_RESIZABLE;
  if (!SDL_CreateWindowAndRenderer("chess",init_width, init_height, flags, &app->window.window, &app->window.renderer))
  {
    return false;
  }

  app->quit = false;
  return true;
}

void application_handle_events(Application* app)
{
  SDL_Event e = {};
  while (SDL_PollEvent(&e))
  {
    switch (e.type)
  	{
      case SDL_EVENT_QUIT:
        app->quit = true;
	     break;
      default:
        break;
  	}
  }
}

const float g_board_area_ratio = 9.0/10.0;

vec2 get_sdl_window_size(SDL_Window* window)
{
  int x,y;
  SDL_GetWindowSize(window,&x,&y);
  return (vec2){.x=(float)x,.y=(float)y};
}

void draw_checkerboard(Window window, Color white, Color black)
{
  Color save_color;
  SDL_GetRenderDrawColor(window.renderer, &save_color.r, &save_color.g, &save_color.b, &save_color.a);

  vec2 window_size = get_sdl_window_size(window.window);
  vec2 board_pos = (vec2){window_size.x*(1.0-g_board_area_ratio)/2,window_size.y*(1.0-g_board_area_ratio)/2};

  float board_size = MIN(window_size.x,window_size.y)*g_board_area_ratio;

  float cell_size = board_size / 8;

  SDL_SetRenderDrawColor(window.renderer, COLOR_ARG(white));
  for (int r = 7; r >= 0; r--)
  {
    for (int c = 0; c < 8; c++)
    {
      if ((r+c)%2==1)
      {
        SDL_RenderFillRect(window.renderer,
          &(SDL_FRect){.x=board_pos.x+c*cell_size,.y=board_pos.y+r*cell_size,.w=cell_size,.h=cell_size});
      }
    }
  }

  SDL_SetRenderDrawColor(window.renderer, COLOR_ARG(black));
  for (int r = 7; r >= 0; r--)
  {
    for (int c = 0; c < 8; c++)
    {
      if ((r+c)%2==0)
      {
        SDL_RenderFillRect(window.renderer,
          &(SDL_FRect){.x=board_pos.x+c*cell_size,.y=board_pos.y+r*cell_size,.w=cell_size,.h=cell_size});
      }
    }
  }

  SDL_SetRenderDrawColor(window.renderer, COLOR_ARG(save_color));
}

void application_draw(Application* app)
{
  SDL_Renderer* renderer = app->window.renderer;
  SDL_SetRenderDrawColor(renderer, 0xaa, 0x66, 0x33, 0xff);
  SDL_RenderClear(renderer);

  draw_checkerboard(app->window,(Color){0x11,0x11,0x11,0xff},(Color){0xaa,0xaa,0xaa,0xff});

  SDL_RenderPresent(renderer);
}
