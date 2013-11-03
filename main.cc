#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include <SDL/SDL.h>
#include <SDL/SDL_framerate.h>

#include "main.h"
#include "exceptions.h"
#include "rng.h"
#include "setup.h"
#include "global.h"
#include "viewport.h"
#include "events.h"
#include "pixels.h"
#include "screenmessage.h"
#include "cursor.h"
#include "state.h"

using namespace std;

Main::Main(Logger& logger, Setup& setup) : logger(logger), setup(setup) {}

int Main::Dispatch() {
    GlobalData* global = new GlobalData;

    try {
        global->setup = &setup;
        global->logger = &logger;

        global->viewport = new Viewport (global);
        global->rng = new Rng;
        global->pixels = new PixelsDiscrete (global);
        global->messages = new ScreenMessage (global,
            global->setup->messages, 5, 5, 255, 255, 255);
        global->dispatcher = new Dispatcher;
        global->event_source = new EventManager;
        global->cursor = new Cursor (global);
        global->state = new State (global);
        global->dispatcher->appendActor (global->state);
        global->dispatcher->appendActor (global->pixels);
        global->dispatcher->appendActor (global->cursor);
        global->dispatcher->appendActor (global->messages);
        global->dispatcher->render ();
        global->viewport->flip ();

        class EDone {};
        FPSmanager fps;
        int frame = 1;
        SDL_initFramerate (&fps);
        SDL_setFramerate (&fps, 120);
        try {while (true) {
            Event evt;
            global->event_source->tick ();
            while (global->event_source->pollEvent (evt)) {
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
        logger.Log(string("FATAL: ") + e.what());
        return 1;
    }
    catch (EBug e) {
        logger.Log(string("BUG: ") + e.what ());
    }
    delete global;

    return 0;
}
