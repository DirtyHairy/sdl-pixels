#ifndef SETUP_H
#define SETUP_H

#include "logger.h"

class Setup {
    public:
        Setup(); 
        Setup (Logger& logger, int argc, char* argv[]);

        bool fullscreen, verbose, shadow;
        int resx, resy, bpp, pixels, messages;

    private:
        void ResetToDefaults();
};

#endif // SETUP_H
