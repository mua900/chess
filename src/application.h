#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <SDL3/SDL.h>
#include "common.h"
#include "chess.h"

typedef struct {
    String piece_set;
} Config;

// tries a couple of expected configuration file names before giving up and returning the default configuration
Config read_configuration(String root_directory);

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} Window;

typedef struct {
    SDL_Texture* piece_textures[CHESS_PIECE_TYPE_COUNT*2];
} Assets;

bool load_piece_set(Assets* assets, String piece_set);

typedef struct Application {
    Window window;

    Config config;
    Assets assets;

    bool quit;
} Application;

bool create_application(Application* app, const char* base_path);
void application_handle_events(Application* app);
void application_draw(Application* app);

#endif
