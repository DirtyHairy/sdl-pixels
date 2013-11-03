#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "screenmessage.h"
#include "viewport.h"

using namespace std;

ScreenMessage::ScreenMessage (GlobalData* global, int n, int x, int y, int r,
   int g, int b, int fade, int live)
:
   global(global), n(n), x(x), y(y), r(r), g(g), b(b), fade(fade), live(live)
{}

void ScreenMessage::tick () {
   if (queue.empty ()) return;
   for (TQueue::iterator i = queue.begin (); i != queue.end (); i++) {
      i->clock++;
   }
   while (!queue.empty () && queue.front ().clock > 2*fade+live)
      queue.pop_front ();
}

void ScreenMessage::clear () {
   queue.clear ();
}

void ScreenMessage::pushMessage (const string& msg) {
   if (queue.size () >= n) queue.pop_front ();
   queue.push_back (Message(msg));
}

void ScreenMessage::render () {
   int ctr(0);
   Uint8 alpha;
   for (TQueue::const_iterator i = queue.begin (); i != queue.end (); i++) {
      if (i->clock <= fade) alpha = (255 * i->clock) / fade;
      if (i->clock > fade && i->clock <= fade + live) alpha = 255;
      if (i->clock > fade + live && i->clock <= 2*fade+live)
         alpha = (255 * (2*fade+live - i->clock)) / fade;
      stringRGBA (global->viewport->surface, x, y + 10*ctr++,
         i->content.c_str (), r, g, b, alpha);
   }
}
