#ifndef EVENTS_H
#define EVENTS_H

#include <map>
#include <list>

using namespace std;

struct Event {
   enum Type {None, Left, Right, Up, Down, Quit, Freeze, Dump, SpeedUp, SpeedDown,
      ChangeRng};
   Type type;

   Event (Type t) : type(t) {}
   Event () : type(None) {}
};

class EventManager {
   public:
      EventManager (int delay=30, int repeat=15);

      void tick ();
      bool pollEvent (Event& evt);

   private:
      typedef map<Event::Type, int> TRepeaters;
      TRepeaters repeaters;
      int delay, repeat;
      typedef list<Event> TQueue;
      TQueue queue;
};

#endif // EVENTS_H
