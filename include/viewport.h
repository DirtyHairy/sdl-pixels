#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <sstream>
#include <SDL.h>

#include "global.h"

using namespace std;

class Viewport {
   public:
      Viewport (GlobalData* global);
      ~Viewport ();

      inline void lock ();
      inline void unlock ();
      void clear ();
      Uint32 mapColor (Uint8 r, Uint8 g, Uint8 b) const;
      inline void putpixel (int x, int y, Uint32 color);
      void flip ();

      SDL_Surface* surface;
      int resx, resy, bpp;
      bool fullscreen, doublebuffered, hardware, shadow;

   private:
      SDL_Surface* real_surface;
      int bytesPerPixel, pitch;
      bool verbose;
      GlobalData* global;
};

inline void Viewport::putpixel (int x, int y, Uint32 color) {
   switch (bytesPerPixel) {
      case 4:
         static_cast<Uint32*>(surface->pixels)[x+y*pitch/4] = color;
         break;
      case 3:
         int idx;
         idx = x*3+y*pitch;
         static_cast<Uint8*>(surface->pixels)[idx]   =
            *reinterpret_cast<Uint8*>(&color);
         static_cast<Uint8*>(surface->pixels)[idx+1] =
            *(reinterpret_cast<Uint8*>(&color) + 1);
         static_cast<Uint8*>(surface->pixels)[idx+2] =
            *(reinterpret_cast<Uint8*>(&color) + 2);
         break;
      case 2:
         static_cast<Uint16*>(surface->pixels)[x+y*pitch/2] = color;
         break;
      case 1:
         static_cast<Uint8*>(surface->pixels)[x+y*pitch] = color;
         break;
      default:
         stringstream ss;
         ss << " unhandled bytes per pixel " << bytesPerPixel;
         throw EFatal (ss.str ());
   }
}

inline void Viewport::lock () {
   if (SDL_MUSTLOCK(surface)) SDL_LockSurface (surface);
}

inline void Viewport::unlock () {
   if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface (surface);
}

#endif // VIEWPORT_H
