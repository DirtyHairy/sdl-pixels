#ifndef SETUP_H
#define SETUP_H

#include <string>

using namespace std;

class Setup {
   public:
      Setup (int argc, char* argv[]);

      bool fullscreen, verbose, shadow;
      int resx, resy, bpp, pixels;

   private:
      struct ParseState {
         ParseState (int argc, char** argv) : argc(argc), argv(argv), iarg(1) {}

         int argc, iarg;
         char** argv;
      };

      void giveHelp (int status, string msg = "") const;
      bool popInt (int& val, ParseState& state) const;
};

#endif // SETUP_H
