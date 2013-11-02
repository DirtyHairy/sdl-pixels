#include "global.h"
#include "viewport.h"
#include "setup.h"
#include "pixels.h"
#include "screenmessage.h"
#include "events.h"
#include "cursor.h"
#include "state.h"

GlobalData::GlobalData () : setup(0), viewport(0), messages(0), rng(0),
   dispatcher(0), pixels(0), event_source(0), cursor(0), state(0)
{}

GlobalData::~GlobalData () {
   if (viewport) delete viewport;
   if (pixels) delete pixels;
   if (messages) delete messages;
   if (rng) delete rng;
   if (dispatcher) delete dispatcher;
   if (event_source) delete event_source;
   if (cursor) delete cursor;
   if (state) delete state;
}
