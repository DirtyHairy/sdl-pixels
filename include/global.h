#ifndef GLOBAL_H
#define GLOBAL_H

#include "rng.h"
#include "dispatcher.h"

class Setup;
class Viewport;
class Pixels;
class ScreenMessage;

struct GlobalData {
   
   GlobalData ();
   ~GlobalData ();

   Setup* setup;
   Viewport* viewport;
   Pixels* pixels;
   ScreenMessage* messages;
   Rng* rng;
   Dispatcher* dispatcher;
};

#endif // GLOBAL_H
