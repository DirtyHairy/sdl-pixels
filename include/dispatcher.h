#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <list>
#include <map>

#include "actor.h"
#include "unique.h"
#include "events.h"

using namespace std;

class Dispatcher : public Actor {
   public:
      Dispatcher ();
      void appendActor (Actor* actor);
      void replaceActor (Actor* old_actor, Actor* new_actor);
      void deleteActor (Actor* actor);
      void deleteActor (Unique::TId id);

      void render ();
      void tick ();
      void receiveEvent (const Event& e);

   private:
      Dispatcher (const Dispatcher&);

      typedef list<Actor*> TActors;
      TActors actors;
      typedef map<Unique::TId, TActors::iterator> TActorsMap;
      TActorsMap actors_map;
};

#endif // DISPATCHER_H
