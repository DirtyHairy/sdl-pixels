#ifndef ACTOR_H
#define ACTOR_H

#include "unique.h"

using namespace std;

class Actor : public Unique {
   public:
      virtual void tick () = 0;
      virtual void render () = 0;
      virtual void receiveEvent () = 0;

   protected:
      Actor ();
};

#endif // ACTOR_H
