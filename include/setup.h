#ifndef SETUP_H
#define SETUP_H

#include "logger.h"

class Setup {
   public:
      Setup (Logger& logger, int argc, char* argv[]);

      bool fullscreen, verbose, shadow;
      int resx, resy, bpp, pixels, messages;
};

#endif // SETUP_H
