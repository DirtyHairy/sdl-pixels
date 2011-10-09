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

void Dispatcher::replaceActor (Actor* old_actor, Actor* new_actor) {
   Unique::TId id = old_actor->getId ();
   if (actors_map.count (id) == 0) {
      cerr << "WARNING: Dispatcher::replaceActor: attempted to replace "
         << "nonexisting actor " << id << " with " << new_actor->getId ()
         << " - appending instead!";
      appendActor (new_actor);
   } else {
      TActors::iterator i = actors_map[id];
      *i = new_actor;
      actors_map.erase (id);
      actors_map[new_actor->getId ()] = i;
   }
}

void Dispatcher::deleteActor (Unique::TId id) {
   if (actors_map.count (id) == 0) {
      cerr << "WARNING: Dispatcher::deleteActor: attempted to delete "
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

void Dispatcher::receiveEvent (const Event& e) {
   for (TActors::iterator i = actors.begin (); i != actors.end (); i++)
      (*i)->receiveEvent (e);
}
