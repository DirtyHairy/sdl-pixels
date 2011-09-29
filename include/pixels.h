#ifndef PIXELS_H
#define PIXELS_H

#include <vector>
#include <SDL.h>

#include "global.h"
#include "events.h"
#include "actor.h"

class Pixels : public Actor {
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
      GlobalData* global;
};

#endif // PIXELS_H
