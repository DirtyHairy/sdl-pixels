#include "global.h"
#include "viewport.h"
#include "setup.h"
#include "pixels.h"
#include "screenmessage.h"

GlobalData::GlobalData () : setup(0), viewport(0), messages(0), rng(0) {}

GlobalData::~GlobalData () {
   if (setup) delete setup;
   if (viewport) delete viewport;
   if (messages) delete messages;
   if (rng) delete rng;
}
