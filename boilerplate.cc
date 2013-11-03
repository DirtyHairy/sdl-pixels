#include <string>
#include <iostream>

#include "SDL.h"

#include "logger.h"
#include "main.h"

using namespace std;

class PlainLogger : public Logger {
    virtual void Log (const string& msg) {
        cout << msg << "\n";
    }
};

int main (int argc, char* argv[]) {
    PlainLogger logger;
    Setup setup(logger, argc, argv);
    Main main(logger, setup);

    if (SDL_Init (SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        cout << "SDL init failed";
        return 1;
    }

    return main.Dispatch();
}
