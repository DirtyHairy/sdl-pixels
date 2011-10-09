#include <string>

#include "state.h"
#include "screenmessage.h"

using namespace std;

State::State (GlobalData* global) : global(global), frozen(false), mode(Static) {}

void State::receiveEvent (const Event& ev) {
   ScreenMessage* messages = global->messages;
   switch (ev.type) {
      case Event::Freeze:
         frozen = !frozen;
         if (frozen) messages->pushMessage ("frozen");
         else messages->pushMessage ("unfrozen");
         break;
      case Event::StartMagnetic:
         if (mode == Static) {
            mode = Magnetic;
            messages->pushMessage ("magnetic mode engaged");
         }
         break;
      case Event::StopMagnetic:
         if (mode == Magnetic) {
            mode = Static;
            messages->pushMessage ("magnetic mode disengaged");
         }
         break;
      case Event::StartFollow:
         if (mode == Static){
            mode = Follow;
            messages->pushMessage ("follow mode engaged");
         }
         break;
      case Event::StopFollow:
         if (mode == Follow) {
            mode = Static;
            messages->pushMessage ("follow mode disengaged");
         }
         break;
      case Event::ChangeRng:
         global->rng->cycle ();
         messages->pushMessage ("rng set to " + global->rng->name ());
         break;
      case Event::Dump:
         messages->pushMessage ("random number generator: " + global->rng-> name ());
         break;
//      case Event::SwitchPixels:
//         Pixels* new_pixels = cyclePixels (global->pixels);
   }
}

bool State::isFrozen () const {return frozen;}

State::ModeT State::getMode () const {return mode;}
