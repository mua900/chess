#include "std_include.h"

#include "pgn.h"
#include "chess.h"
#include "application.h"

int main(int argc, char** argv)
{
    Application application;
    const char* base_path = SDL_GetBasePath();
    
    if (!create_application(&application, base_path))
    {
        return 1;
    }

    while (!application.quit)
    {
        application_handle_events(&application);
        application_draw(&application);
    }

    return 0;
}
