#ifndef GLOBAL_H
#define GLOBAL_H

#include "rng.h"
#include "dispatcher.h"

class Setup;
class Viewport;
class Pixels;
class ScreenMessage;
class EventManager;
class Cursor;
class State;
class Logger;

struct GlobalData {
   
   GlobalData ();
   ~GlobalData ();

   Setup* setup;
   Viewport* viewport;
   Pixels* pixels;
   ScreenMessage* messages;
   Rng* rng;
   Dispatcher* dispatcher;
   EventManager* event_source;
   Cursor* cursor;
   State* state;
   Logger* logger;
};

#endif // GLOBAL_H
