#include <sstream>

#include "pixels.h"

using namespace std;

Pixels::Pixels (GlobalData* global)
:
   global(global), viewport(global->viewport),
   pixels(TPixels(global->setup->pixels)), stepn(1), steps(1),
   stepw(1), stepe(1), frozen(false), speed(4), rng(global->rng)
{
   for (int i = 0; i <= global->setup->pixels-1; i++) pixels[i] = Pixel(
      rng->rnd () % viewport->resx,
      rng->rnd () % viewport->resy,
      viewport->mapColor (rng->rnd () % 256, rng->rnd () % 256, rng->rnd () % 256)
   );
}

void Pixels::render () {
   viewport->lock ();
   for (TPixels::const_iterator i = pixels.begin (); i != pixels.end (); i++)
      viewport->putpixel (i->x, i->y, i->color);
   viewport->unlock ();
}

void Pixels::tick () {
   Uint32 rval;
   if (frozen) return;
   int shift = rng->bits - 1;
   int mshift = rng->bits - 2;
   for (TPixels::iterator i = pixels.begin (); i != pixels.end (); i++) {
      for (int j = 1; j <= speed; j++) {
         if (shift > mshift) {
            rval = rng->rnd ();
            shift = 0;
         }
         switch ((rval >> shift) & 3) {
            case 0:
               if ((i->x += stepe) >= viewport->resx) i->x -= stepe;
               break;
            case 1:
               if ((i->x -= stepw) < 0) i->x += stepw;
               break;
            case 2:
               if ((i->y += steps) >= viewport->resy) i->y -= steps;
               break;
            case 3:
               if ((i->y -= stepn) < 0) i->y += stepn;
               break;
         }
         shift += 2;
      }
   }
}

void Pixels::receiveEvent (const Event& evt) {
   stringstream ss;
   switch (evt.type) {
      case Event::Left:
         if (stepe > 1) {
            stepe--;
            ss << "step width east: " << stepe;
            global->messages->pushMessage (ss.str ());
         } else {
            stepw++;
            ss << "step width west: " << stepw;
            global->messages->pushMessage (ss.str ());
         }
         break;
      case Event::Right:
         if (stepw > 1) {
            stepw--; 
            ss << "step width west: " << stepw;
            global->messages->pushMessage (ss.str ());
         } else {
            stepe++;
            ss << "step width east: " << stepe;
            global->messages->pushMessage (ss.str ());
         }
         break;
      case Event::Up:
         if (steps > 1) {
            steps--;
            ss << "step width south: " << steps;
            global->messages->pushMessage (ss.str ());
         } else {
            stepn++;
            ss << "step width north: " << stepn;
            global->messages->pushMessage (ss.str ());
         }
         break;
      case Event::Down:
         if (stepn > 1) {
            stepn--;
            ss << "step width north: " << stepn;
            global->messages->pushMessage (ss.str ());
         } else {
            steps++;
            ss << "step width south: " << steps;
            global->messages->pushMessage (ss.str ());
         }
         break;
      case Event::SpeedUp:
         speed++;
         ss << "speed: " << speed;
         global->messages->pushMessage (ss.str ());
         break;
      case Event::SpeedDown:
         if (speed > 1) speed--;
         ss << "speed: " << speed;
         global->messages->pushMessage (ss.str ());
         break;
      case Event::Freeze:
         if (frozen) global->messages->pushMessage ("unfrozen");
         else global->messages->pushMessage ("frozen");
         frozen = !frozen;
         break;
      case Event::ChangeRng:
         rng->cycle ();
         global->messages->pushMessage ("rng set to " + rng->name ());
         break;
      case Event::Dump:
         ss <<    "north: " << stepn << "   west: " << stepw;
         ss << "   south: " << steps << "   east: " << stepe;
         ss << "   speed: " << speed;
         global->messages->pushMessage (ss.str ());
         global->messages->pushMessage ("random number generator: " + rng-> name ());
         break;
   }
}

