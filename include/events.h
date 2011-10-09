#ifndef EVENTS_H
#define EVENTS_H

#include <map>
#include <list>

using namespace std;

struct Event {
   enum Type {None, Left, Right, Up, Down, Quit, Freeze, Dump, SpeedUp, SpeedDown,
      ChangeRng, StartMagnetic, StopMagnetic, StartFollow, StopFollow, 
      SwitchPixels};
   Type type;

   Event (Type t) : type(t) {}
   Event () : type(None) {}
};

class EventManager {
   public:
      EventManager (int delay=30, int repeat=15);

      void tick ();
      bool pollEvent (Event& evt);
      void getMouseCoords (int& x, int& y) const;

   private:
      typedef map<Event::Type, int> TRepeaters;
      TRepeaters repeaters;
      int delay, repeat;
      typedef list<Event> TQueue;
      TQueue queue;

      int mouse_x, mouse_y;
};

#endif // EVENTS_H
