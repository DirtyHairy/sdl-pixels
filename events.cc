#include <SDL.h>

#include "events.h"

using namespace std;

EventManager::EventManager (int delay, int repeat) :
   delay(delay), repeat(repeat) {}

bool EventManager::pollEvent (Event& evt) {
   if (queue.empty ()) return false;
   evt = queue.front ();
   queue.pop_front ();
   return true;
}

void EventManager::tick () {
   SDL_Event evt;
   while (SDL_PollEvent (&evt)) {
      bool valid;
      Event::Type t;
      switch (evt.type) {
         case SDL_QUIT:
            queue.push_back (Event (Event::Quit));
            break;
         case SDL_KEYDOWN:
         case SDL_KEYUP:
            valid = true;
            switch (evt.key.keysym.sym) {
               case SDLK_q:
                  t = Event::Quit;
                  break;
               case SDLK_UP:
                  t = Event::Up;
                  break;
               case SDLK_DOWN:
                  t = Event::Down;
                  break;
               case SDLK_LEFT:
                  t = Event::Left;
                  break;
               case SDLK_RIGHT:
                  t = Event::Right;
                  break;
               case (SDLK_f):
                  t = Event::Freeze;
                  break;
               case (SDLK_d):
                  t = Event::Dump;
                  break;
               case (SDLK_PLUS):
                  t = Event::SpeedUp;
                  break;
               case (SDLK_MINUS):
                  t = Event::SpeedDown;
                  break;
               case (SDLK_r):
                  t = Event::ChangeRng;
                  break;
               default:
                  valid = false;
            }
            if (valid) {
               if (evt.key.type == SDL_KEYDOWN) {
                  queue.push_back (Event (t));
                  repeaters[t] = 0;
               } else {
                  repeaters.erase (t);
               }
            }
            break;
         default:
            break;
      }
  }
  for (TRepeaters::iterator i = repeaters.begin (); i != repeaters.end (); i++) {
      if (i->second >= delay && ((i->second - delay) % repeat == 0))
         queue.push_back (i->first);
      i->second++;
  }
}

