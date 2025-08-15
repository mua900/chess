#include "std_include.h"

#include "pgn.h"
#include "chess.h"
#include "application.h"


int main(int argc, char** argv)
{
    Application application;
    if (!create_application(&application))
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
