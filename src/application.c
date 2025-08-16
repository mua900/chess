#include "application.h"

bool create_application(Application* app, const char* base_path)
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

  app->config = read_configuration(make_string(base_path));
  load_piece_set(&app->assets, app->config.piece_set);

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
      case SDL_EVENT_KEY_DOWN:
      {
        SDL_KeyboardEvent keyboard = e.key;
        switch (keyboard.scancode)
        {
          case SDL_SCANCODE_ESCAPE:
            app->quit = true;
            break;
        }
        break;
      }
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

void draw_checkerboard(Window window, Color white, Color black, vec2 pos, float cell_size)
{
  Color save_color;
  SDL_GetRenderDrawColor(window.renderer, &save_color.r, &save_color.g, &save_color.b, &save_color.a);

  SDL_SetRenderDrawColor(window.renderer, COLOR_ARG(white));
  for (int r = 7; r >= 0; r--)
  {
    for (int c = 0; c < 8; c++)
    {
      if ((r+c)%2==1)
      {
        SDL_RenderFillRect(window.renderer,
          &(SDL_FRect){.x=pos.x+c*cell_size,.y=pos.y+r*cell_size,.w=cell_size,.h=cell_size});
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
          &(SDL_FRect){.x=pos.x+c*cell_size,.y=pos.y+r*cell_size,.w=cell_size,.h=cell_size});
      }
    }
  }

  SDL_SetRenderDrawColor(window.renderer, COLOR_ARG(save_color));
}

void application_draw(Application* app)
{
  SDL_Renderer* renderer = app->window.renderer;
  Window window = app->window;
  SDL_SetRenderDrawColor(renderer, 0xaa, 0x66, 0x33, 0xff);
  SDL_RenderClear(renderer);

  vec2 window_size = get_sdl_window_size(window.window);
  float board_size = MIN(window_size.x,window_size.y)*g_board_area_ratio;
  vec2 board_pos = (vec2){(window_size.x-board_size)/2,(window_size.y-board_size)/2};

  float cell_size = board_size / 8;

  draw_checkerboard(window,(Color){0x11,0x11,0x11,0xff},(Color){0xaa,0xaa,0xaa,0xff},board_pos,cell_size);

  SDL_RenderPresent(renderer);
}

// @todo complete
bool parse_config(String configuration, Config* conf)
{
  String key = STRING_EMPTY;
  String value = STRING_EMPTY;
  int key_start = 0;
  int value_start = 0;
  for (int cursor = 0; cursor < configuration.size; cursor++)
  {
    char c = configuration.data[cursor];

    #define DELIMETER ':'
    if (c == DELIMETER)
    {
      key = string_trim(string_slice(configuration, key_start, cursor));
      value_start = cursor+1;
    }
    else if (c == '\n')
    {
      value = string_trim(string_slice(configuration, value_start, cursor));
      key_start = cursor+1;

      if (string_compare(key,MAKE_STRING("piece_set")))
      {
        conf->piece_set = value;
        
      }
    }
  }

  return true;
}

const Config DefaultConfiguration = (Config){
  .piece_set = MAKE_STRING("alpha")
};

SDL_EnumerationResult find_and_load_config(void* user_data, const char* dir_name, const char* fname)
{
  Config* config = (Config*)user_data;

  const String config_file_names[] = {
    MAKE_STRING("caro.conf"),
    MAKE_STRING("config.txt"),
    MAKE_STRING("config"),
  };

  String file_name = make_string(fname);

  for (int i = 0; i < ARRAY_SIZE(config_file_names); i++)
  {
    String conf_file_name = config_file_names[i];
    if (string_compare(file_name, conf_file_name))
    {
      bool load_success;
      File file = load_file(fname, &load_success);

      LOG_MSGF("Configuration file %s loaded", fname);
      if (load_success)
      {
        bool parse_success = parse_config(file, config);

        unload_file(file);
        break;
      }

      return SDL_ENUM_SUCCESS;
    }
  }

  return SDL_ENUM_CONTINUE;
}


Config read_configuration(String base_path)
{
  Config config = DefaultConfiguration;

  SDL_EnumerateDirectory(SDL_GetBasePath(), find_and_load_config, &config);

  return config;
}

SDL_EnumerationResult load_piece(void* userdata, const char* dirname, const char* fname)
{
  Assets* assets = (Assets*)userdata;

  const char* piece_names[] = {
    "wK.png","wQ.png","wR.png","wB.png","wN.png","wP.png",
    "bK.png","bQ.png","bR.png","bB.png","bN.png","bP.png"
  };

  for (int i = 0; i < ARRAY_SIZE(piece_names); i++)
  {
    if (strcmp(piece_names[i],fname))
    {
      
    }
  }

  return SDL_ENUM_CONTINUE;
}

bool load_piece_set(Assets* assets, String piece_set)
{
  String_Builder sb = make_string_builder(256);
  sb_append(&sb, MAKE_STRING("../asset/"));
  sb_append(&sb, piece_set);
  sb_append_char(&sb, '/');

  SDL_EnumerateDirectory(sb_c_string(&sb), load_piece, assets);

  sb_free(&sb);
  return true;
}
