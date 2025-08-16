#include "std_include.h"

#include "pgn.h"
#include "chess.h"
#include "application.h"

int main(int argc, char** argv)
{
    Application application;
    Config config = read_configuration(make_string(argv[0]));
    if (!create_application(&application,&config))
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
