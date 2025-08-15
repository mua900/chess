#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <SDL3/SDL.h>
#include "common.h"

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} Window;

typedef struct Application {
    Window window;

    bool quit;
} Application;

bool create_application(Application* app);
void application_handle_events(Application* app);
void application_draw(Application* app);

#endif
