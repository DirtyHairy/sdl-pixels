#include "global.h"
#include "viewport.h"
#include "setup.h"
#include "pixels.h"
#include "screenmessage.h"

GlobalData::GlobalData () : setup(0), viewport(0), messages(0), rng(0),
   dispatcher(0), pixels(0)
{}

GlobalData::~GlobalData () {
   if (setup) delete setup;
   if (viewport) delete viewport;
   if (pixels) delete pixels;
   if (messages) delete messages;
   if (rng) delete rng;
   if (dispatcher) delete dispatcher;
}
