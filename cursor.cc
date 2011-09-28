#include <SDL/SDL_gfxPrimitives.h>

#include "cursor.h"
#include "events.h"

Cursor::Cursor (GlobalData* global) : global(global) {}

void Cursor::render () {
   int x, y;
   global->event_source->getMouseCoords (x, y);
   filledCircleRGBA (global->viewport->surface, x, y, 8, 200, 200, 200, 100);
}

#include "cursor.h"
