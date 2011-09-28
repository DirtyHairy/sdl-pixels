#ifndef CURSOR_H
#define CURSOR_H

#include "actor.h"
#include "global.h"
#include "viewport.h"

class Cursor : public Actor {
   public:
      Cursor (GlobalData* global);
      void render ();

   private:
      GlobalData* global;
};

#endif // CURSOR_H
