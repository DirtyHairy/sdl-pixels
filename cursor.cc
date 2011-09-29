#include <SDL/SDL_gfxPrimitives.h>

#include "cursor.h"
#include "events.h"
#include "state.h"

Cursor::Cursor (GlobalData* global) : global(global) {}

void Cursor::render () {
   int x, y;
   Uint8 r, g, b, a;
   global->event_source->getMouseCoords (x, y);
   switch (global->state->getMode ()) {
      case State::Static:
         r = g = b = 200;
         a = 50;
         break;
      case State::Magnetic:
         r = 200;
         g = b = 0;
         a = 150;
         break;
      case State::Follow:
         g = 200;
         r = b = 0;
         a = 150;
         break;
   }
   filledCircleRGBA (global->viewport->surface, x, y, 8, r, g, b, a);
}

#include "cursor.h"
