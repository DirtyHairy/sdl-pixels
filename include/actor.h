#ifndef ACTOR_H
#define ACTOR_H

#include "unique.h"
#include "events.h"

using namespace std;

class Actor : public Unique {
   public:
      virtual void tick ();
      virtual void render ();
      virtual void receiveEvent (const Event&);

   protected:
      Actor ();
};

#endif // ACTOR_H
