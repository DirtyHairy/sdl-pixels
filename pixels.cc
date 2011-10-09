#include <sstream>
#include <cmath>

#include "pixels.h"
#include "viewport.h"
#include "rng.h"
#include "screenmessage.h"
#include "setup.h"
#include "state.h"

using namespace std;

namespace {
   const float pi = 2 * asin (1);
}

// *** Pixels

Pixels::Pixels (GlobalData* global) : lobal(global),
   pixels(TPixels(global->setup->pixels)), vecx(0), vecy(0), speed(4)
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

// Pixels

// *** PixelsDiscrete

const string PixelsDiscrete::description () const {
   return "Movement mode: discrete";
}

PixelsDiscrete::PixelsDiscrete (GlobalData* global) : Pixels(global), 
   stepn(1), steps(1), stepw(1), stepe(1)
{}

PixelsDiscrete::PixelsDiscrete (const Pixels& src) : Pixels(src),
   stepn(1), steps(1), stepw(1), stepe(1)
{
   if (vecx > 0) stepe =  vecx + 1;
   if (vecx < 0) stepw = -vecx + 1;
   if (vecy > 0) stepn =  vecy + 1;
   if (vecy < 0) steps = -vecy + 1;
}

void PixelsDiscrete::tick () {
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
                     if (step > 7) step = 7;
                     if ((i->x += step) > xmax) i->x -= step;
                     break;
                  case 1:
                     step = (px < i->x)?((i->x - px) / 10 + 1):1;
                     if (step > 7) step = 7;
                     if ((i->x -= step) < 0) i->x += step;
                     break;
                  case 2:  
                     step = (py > i->y)?((py - i->y) / 10 + 1):1;
                     if (step > 7) step = 7;
                     if ((i->y += step) > ymax) i->y -= step;
                     break;
                  case 3:
                     step = (py < i->y)?((i->y - py) / 10 + 1):1;
                     if (step > 7) step = 7;
                     if ((i->y -= step) < 0) i->y += step;
                     break;
               }
               break;
         }
         shift += 2;
      }
   }
}  

void PixelsDiscrete::receiveEvent (const Event& evt) {
   stringstream ss;
   ScreenMessage* messages = global->messages;
   bool recalc = false;
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
         recalc = true;
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
         recalc = true;
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
         recalc = true;
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
         recalc = true;
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
      case Event::Dump:
         messages->pushMessage (description ());
         ss <<    "north: " << stepn << "   west: " << stepw;
         ss << "   south: " << steps << "   east: " << stepe;
         ss << "   speed: " << speed;
         messages->pushMessage (ss.str ());
         break;
   }
   if (recalc) {
      if (stepe > 1) vecx = stepe;
      else if (stepw > 1) vecx = -stepw;
      else vecx = 0;
      if (stepn > 1) vecy = stepn;
      else if (steps > 1) vecy = -steps;
      else vecy = 0;
   }
}

// ### PixelsDiscrete

// PixelsContinuous

const string PixelsContinuous::description () const {
   return "Movement mode: continuous";
}

PixelsContinuous::PixelsContinuous (GlobalData* global) : Pixels(global),
   length(0), rotxx(1), rotxy(0), rotyx(0), rotyy(1)
{}

PixelsContinuous::PixelsContinuous (const Pixels& src) : Pixels(src),
   rotxx(1), rotxy(0), rotyx(0), rotyy(1)
{
   recalcPars ();
}

void PixelsContinuous::recalcPars () {
   length = sqrt (vecx*vecx + vecy*vecy);
   if (length > 0) {
      rotxx = rotyy = vecx / length;
      rotyx = vecy / length; rotxy = - rotyx;
   } else {
      rotxx = 1; rotyy = 1; rotxy = rotyx = 0;
   }
}

void PixelsContinuous::tick () {
   Rng* rng = global->rng;
   int resx = global->viewport->resx, resy = global->viewport->resy;
   float sqrt_speed = sqrt (speed);
   if (global->state->isFrozen ()) return;

   for (TPixels::iterator i = pixels.begin (); i < pixels.end (); i++) {
      float phi = rng->rnd_unit () * 2 * pi;
      float dx = cos (phi) * sqrt_speed, dy = sin (phi) * sqrt_speed;
      if (dx > 0) dx += length * speed / 2;
      int x = floor (i->x + rotxx * dx + rotxy * dy + 0.5);
      int y = floor (i->y - rotyx * dx - rotyy * dy + 0.5);
      if ((x >= 0) && (x < resx)) i->x = x;
      if ((y >= 0) && (y < resy)) i->y = y;
   }
}

void PixelsContinuous::receiveEvent (const Event& evt) {
   stringstream ss;
   ScreenMessage* messages = global->messages;
   bool recalc = false;

   switch (evt.type) {
      case Event::Left:
         vecx--;
         recalc = true;
         break;
      case Event::Right:
         vecx++;
         recalc = true;
         break;
      case Event::Up:
         vecy++;
         recalc = true;
         break;
      case Event::Down:
         vecy--;
         recalc = true;
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
      case Event::Dump:
         messages->pushMessage (description ());
         ss <<    "speed: " << speed << "   movement vector: ("
            << vecx << ", " << vecy << ")";
         messages->pushMessage (ss.str ());
         break;
   }

   if (recalc) {
      ss << "movement vector: (" << vecx << ", " << vecy << ")";
      messages->pushMessage (ss.str ());
      recalcPars ();
   }
}

// ### PixelsContinuous

Pixels* cyclePixels (const Pixels* old) {
   if (dynamic_cast<const PixelsDiscrete*>(old))
      return new PixelsContinuous (*old);
   return new PixelsDiscrete (*old);
}
