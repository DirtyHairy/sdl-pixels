#include <SDL.h>

#include "events.h"

using namespace std;

EventManager::EventManager (int delay, int repeat) :
   delay(delay), repeat(repeat)
{
   SDL_GetMouseState (&mouse_x, &mouse_y);
}

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
         case SDL_MOUSEBUTTONDOWN:
            switch (evt.button.button) {
               case SDL_BUTTON_LEFT:
                  queue.push_back (Event (Event::StartMagnetic));
                  break;
               case SDL_BUTTON_RIGHT:
                  queue.push_back (Event (Event::StartFollow));
                  break;
            }
            break;
         case SDL_MOUSEBUTTONUP:
            switch (evt.button.button) {
               case SDL_BUTTON_LEFT:
                  queue.push_back (Event (Event::StopMagnetic));
                  break;
               case SDL_BUTTON_RIGHT:
                  queue.push_back (Event (Event::StopFollow));
                  break;
               }
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
               case (SDLK_p):
                  t = Event::SwitchPixels;
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
  SDL_GetMouseState (&mouse_x, &mouse_y);
}

void EventManager::getMouseCoords (int& x, int& y) const {
   x = mouse_x;
   y = mouse_y;
}
