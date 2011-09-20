#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include <SDL.h>
#include <SDL_framerate.h>

#include "exceptions.h"
#include "rng.h"
#include "setup.h"
#include "global.h"
#include "viewport.h"
#include "events.h"
#include "pixels.h"
#include "screenmessage.h"

using namespace std;


int main (int argc, char* argv[]) {
   GlobalData* global = new GlobalData;
   try {
      global->setup = new Setup(argc, argv);
      global->viewport = new Viewport (global);
      global->rng = new Rng;
      global->pixels = new Pixels(global);
      global->messages = new ScreenMessage (global, 5, 5, 5, 255, 255, 255);
      global->dispatcher = new Dispatcher;
      global->dispatcher->appendActor (global->pixels);
      global->dispatcher->appendActor (global->messages);
      global->dispatcher->render ();
      global->viewport->flip ();

      EventManager eventSource;

      class EDone {};
      FPSmanager fps;
      int frame = 1;
      SDL_initFramerate (&fps);
      SDL_setFramerate (&fps, 120);
      try {while (true) {
         Event evt;
         eventSource.tick ();
         while (eventSource.pollEvent (evt)) {
            if (evt.type == Event::Quit) throw EDone ();
            global->dispatcher->receiveEvent (evt);
         }

         global->dispatcher->tick ();

         if (frame++ == 2) {
            frame = 0;
            global->viewport->clear ();
            global->dispatcher->render ();
            global->viewport->flip ();
         }

         SDL_framerateDelay (&fps);
      }}
      catch (EDone) {}
   }
   catch (EFatal e) {
      cout << "FATAL: " << e.what () << "\n";
      return 1;
   }
   catch (EBug e) {
      cout << "BUG: " << e.what () << "\n";
   }
   delete global;
}
