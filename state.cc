#include <string>

#include "state.h"
#include "screenmessage.h"

using namespace std;

State::State (GlobalData* global) : global(global), frozen(false), mode(Static) {}

void State::receiveEvent (const Event& ev) {
   switch (ev.type) {
      case Event::Freeze:
         frozen = !frozen;
         if (frozen) global->messages->pushMessage ("frozen");
         else global->messages->pushMessage ("unfrozen");
         break;
      case Event::StartMagnetic:
         if (mode == Static) {
            mode = Magnetic;
            global->messages->pushMessage ("magnetic mode engaged");
         }
         break;
      case Event::StopMagnetic:
         if (mode == Magnetic) {
            mode = Static;
            global->messages->pushMessage ("magnetic mode disengaged");
         }
         break;
      case Event::StartFollow:
         if (mode == Static){
            mode = Follow;
            global->messages->pushMessage ("follow mode engaged");
         }
         break;
      case Event::StopFollow:
         if (mode == Follow) {
            mode = Static;
            global->messages->pushMessage ("follow mode disengaged");
         }
         break;
   }
}

bool State::isFrozen () const {return frozen;}

State::ModeT State::getMode () const {return mode;}
