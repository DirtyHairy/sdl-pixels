#ifndef SCREEN_MESSAGE_H
#define SCREEN_MESSAGE_H

#include <list>
#include <string>

#include "global.h"
#include "actor.h"

using namespace std;

class ScreenMessage : public Actor {
   public:
      ScreenMessage (GlobalData* global, int n, int x, int y, int r,
         int g, int b, int fade=30, int live=60);
      ~ScreenMessage () {};
      void pushMessage (const string& msg);
      void clear ();
      void tick ();
      void render ();

   private:
      struct Message {
         Message (const string& c) : content(c), clock(0) {}
         string content;
         int clock;
      };

      typedef list<Message> TQueue;
      TQueue queue;
      int n, x, y, r, g, b, fade, live;
      GlobalData* global;
};


#endif // SCREEN_MESSAGE_H
