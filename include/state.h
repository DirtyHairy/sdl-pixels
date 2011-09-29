#ifndef STATE_H
#define STATE_H

#include "actor.h"
#include "events.h"
#include "global.h"

class State : public Actor {
   public:
      State (GlobalData* global);
      void receiveEvent (const Event&);

      enum ModeT {Static, Follow, Magnetic};

      bool isFrozen () const;
      ModeT getMode () const;


   private:
      bool frozen;
      ModeT mode;
      GlobalData* global;
};

#endif // STATE_H
