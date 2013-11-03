#ifndef PIXELS_H
#define PIXELS_H

#include <vector>
#include <SDL/SDL.h>

#include "global.h"
#include "events.h"
#include "actor.h"

class Pixels : public Actor {
   public:
      Pixels (GlobalData*);

      void render ();
      virtual const string description () const = 0;

   protected:
      struct Pixel {
         int x, y;
         Uint32 color;

         Pixel () : x(0), y(0), color(0) {}
         Pixel (int x, int y, Uint32 color) : x(x), y(y), color(color) {}
      };

      typedef vector<Pixel> TPixels;
      TPixels pixels;
      int speed, vecx, vecy;
      GlobalData* global;

   private:
      Pixels ();
};

class PixelsDiscrete : public Pixels {
   public:
      PixelsDiscrete (GlobalData*);
      PixelsDiscrete (const Pixels&);

      void tick ();      
      void receiveEvent (const Event&);
      const string description () const;

   private:
      int stepn, steps, stepw, stepe;
};

class PixelsContinuous : public Pixels {
   public:
      PixelsContinuous (GlobalData*);
      PixelsContinuous (const Pixels&);

      void tick ();
      void receiveEvent (const Event&);
      const string description () const;

   private:
      float length;
      float rotxx, rotxy, rotyx, rotyy;
      
      void recalcPars ();
};

Pixels* cyclePixels (const Pixels* old);

#endif // PIXELS_H
