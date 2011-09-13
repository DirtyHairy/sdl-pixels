#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <list>
#include <map>

#include "actor.h"
#include "unique.h"

using namespace std;

class Dispatcher : public Actor {
   public:
      Dispatcher ();
      void appendActor (Actor* actor);
      void deleteActor (Actor* actor);
      void deleteActor (const Unique::TId id);

      void render ();
      void tick ();
      void receiveEvent ();

   private:
      Dispatcher (const Dispatcher&);

      typedef list<Actor*> TActors;
      TActors actors;
      typedef map<Unique::TId, TActors::iterator> TActorsMap;
      TActorsMap actors_map;
};

#endif // DISPATCHER_H
