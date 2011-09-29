#include <sstream>

#include "pixels.h"
#include "viewport.h"
#include "rng.h"
#include "screenmessage.h"
#include "setup.h"
#include "state.h"

using namespace std;

Pixels::Pixels (GlobalData* global)
:
   global(global), pixels(TPixels(global->setup->pixels)), stepn(1), steps(1),
   stepw(1), stepe(1), speed(4)
{
   Rng* rng = global->rng;
   Viewport* viewport = global->viewport;
   for (int i = 0; i <= global->setup->pixels-1; i++) pixels[i] = Pixel(
      rng->rnd () % viewport->resx,
      rng->rnd () % viewport->resy,
      viewport->mapColor (rng->rnd () % 256, rng->rnd () % 256, rng->rnd () % 256)
   );
}

void Pixels::render () {
   Viewport* viewport = global->viewport;
   viewport->lock ();
   for (TPixels::const_iterator i = pixels.begin (); i != pixels.end (); i++)
      viewport->putpixel (i->x, i->y, i->color);
   viewport->unlock ();
}

void Pixels::tick () {
   if (global->state->isFrozen ()) return;

   Uint32 rval;
   Rng* rng = global->rng;
   Viewport* viewport = global->viewport;
   int shift = rng->bits - 1;
   int mshift = rng->bits - 2;

   State::ModeT mode = global->state->getMode ();
   int px, py, step, xmax=viewport->resx-1, ymax=viewport->resy-1;
   global->event_source->getMouseCoords (px, py);

   for (int j = 1; j <= speed; j++) {
      for (TPixels::iterator i = pixels.begin (); i != pixels.end (); i++) {
         if (shift > mshift) {
            rval = rng->rnd ();
            shift = 0;
         }
         switch (mode) {
            case State::Static:
               switch ((rval >> shift) & 3) {
                  case 0:
                     if ((i->x += stepe) >= xmax) i->x -= stepe;
                     break;
                  case 1:
                     if ((i->x -= stepw) < 0) i->x += stepw;
                     break;
                  case 2:
                     if ((i->y += steps) >= ymax) i->y -= steps;
                     break;
                  case 3:
                     if ((i->y -= stepn) < 0) i->y += stepn;
                     break;
               }
               break;
            case State::Magnetic:
               switch ((rval >> shift) & 3) {
                  case 0:
                     step = (px > i->x)?((px - i->x) / 10 + 1):1;
                     step = (step>5)?10:step;
                     if ((i->x += step) > xmax) i->x -= step;
                     break;
                  case 1:
                     step = (px < i->x)?((i->x - px) / 10 + 1):1;
                     step = (step>5)?10:step;
                     if ((i->x -= step) < 0) i->x += step;
                     break;
                  case 2:  
                     step = (py > i->y)?((py - i->y) / 10 + 1):1;
                     step = (step>5)?10:step;
                     if ((i->y += step) > ymax) i->y -= step;
                     break;
                  case 3:
                     step = (py < i->y)?((i->y - py) / 10 + 1):1;
                     step = (step>5)?10:step;
                     if ((i->y -= step) < 0) i->y += step;
                     break;
               }
               break;
         }
         shift += 2;
      }
   }
}  

void Pixels::receiveEvent (const Event& evt) {
   stringstream ss;
   ScreenMessage* messages = global->messages;
   switch (evt.type) {
      case Event::Left:
         if (stepe > 1) {
            stepe--;
            ss << "step width east: " << stepe;
            messages->pushMessage (ss.str ());
         } else {
            stepw++;
            ss << "step width west: " << stepw;
            messages->pushMessage (ss.str ());
         }
         break;
      case Event::Right:
         if (stepw > 1) {
            stepw--; 
            ss << "step width west: " << stepw;
            messages->pushMessage (ss.str ());
         } else {
            stepe++;
            ss << "step width east: " << stepe;
            messages->pushMessage (ss.str ());
         }
         break;
      case Event::Up:
         if (steps > 1) {
            steps--;
            ss << "step width south: " << steps;
            messages->pushMessage (ss.str ());
         } else {
            stepn++;
            ss << "step width north: " << stepn;
            messages->pushMessage (ss.str ());
         }
         break;
      case Event::Down:
         if (stepn > 1) {
            stepn--;
            ss << "step width north: " << stepn;
            messages->pushMessage (ss.str ());
         } else {
            steps++;
            ss << "step width south: " << steps;
            messages->pushMessage (ss.str ());
         }
         break;
      case Event::SpeedUp:
         speed++;
         ss << "speed: " << speed;
         messages->pushMessage (ss.str ());
         break;
      case Event::SpeedDown:
         if (speed > 1) speed--;
         ss << "speed: " << speed;
         messages->pushMessage (ss.str ());
         break;
      case Event::ChangeRng:
         global->rng->cycle ();
         messages->pushMessage ("rng set to " + global->rng->name ());
         break;
      case Event::Dump:
         ss <<    "north: " << stepn << "   west: " << stepw;
         ss << "   south: " << steps << "   east: " << stepe;
         ss << "   speed: " << speed;
         messages->pushMessage (ss.str ());
         messages->pushMessage ("random number generator: " + global->rng-> name ());
         break;
   }
}

