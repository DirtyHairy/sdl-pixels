#include <iostream>

#include "dispatcher.h"

using namespace std;

// Dispatcher

Dispatcher::Dispatcher () {}

void Dispatcher::appendActor (Actor* actor) {
   Unique::TId id = actor->getId ();
   if (actors_map.count (id) > 0) return;
   actors.push_back (actor);
   actors_map[id] = (--actors.end ());
}

void Dispatcher::deleteActor (Unique::TId id) {
   if (actors_map.count (id) == 0) {
      cout << "WARNING: Dispatcher::deleteActor: attempted to delete "
         << "nonexisting actor " << id << "!\n";
      return;
   }
   actors.erase (actors_map[id]);
   actors_map.erase (id);
}

void Dispatcher::deleteActor (Actor* actor) {
   deleteActor (actor->getId ());
}

void Dispatcher::tick () {
   for (TActors::iterator i = actors.begin (); i != actors.end (); i++)
      (*i)->tick ();
}

void Dispatcher::render () {
   for (TActors::iterator i = actors.begin (); i != actors.end (); i++)
      (*i)->render ();
}

void Dispatcher::receiveEvent () {
   for (TActors::iterator i = actors.begin (); i != actors.end (); i++)
      (*i)->receiveEvent ();
}
