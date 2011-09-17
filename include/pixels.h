#ifndef PIXELS_H
#define PIXELS_H

#include <vector>
#include <SDL.h>

#include "global.h"
#include "viewport.h"
#include "rng.h"
#include "events.h"
#include "screenmessage.h"
#include "setup.h"

class Pixels {
   public:
      Pixels (GlobalData*);
      void tick ();
      void render ();
      void receiveEvent (const Event& e);

   private:
      struct Pixel {
         int x, y;
         Uint32 color;

         Pixel () : x(0), y(0), color(0) {}
         Pixel (int x, int y, Uint32 color) : x(x), y(y), color(color) {}
      };

      typedef vector<Pixel> TPixels;
      TPixels pixels;
      int stepn, steps, stepw, stepe, speed;
      bool frozen;
      GlobalData* global;
      Viewport* viewport;
      Rng* rng;
};

#endif // PIXELS_H
