#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <exception>
#include <vector>
#include <list>
#include <map>
#include <utility>

#include <SDL.h>
#include <SDL_framerate.h>
#include <SDL_gfxPrimitives.h>

using namespace std;

class EPixelsExcpt: public exception {
   public:
      EPixelsExcpt (const string& msg) : msg(msg) {}
      ~EPixelsExcpt () throw () {}
      const char* what () {return msg.c_str ();}

   protected:
      string msg;
};

class EFatal: public EPixelsExcpt {
   public:
      EFatal (const string& msg) : EPixelsExcpt(msg) {}
};

class EBug: public EPixelsExcpt {
   public:
      EBug (const string& msg) : EPixelsExcpt(msg) {}
};

class Rng {
   public:
      enum Type {Default, MultiplyWithCarry, LC1, LC2};

      Rng (const Type t=Default);
      const string name () const;
      inline const Uint32 rnd ();
      void reset (const Type t);
      void cycle ();

      int bits;
      Uint32 max;

   private:
      Uint32 state[2];
      Type type;

      const int getBitsGuaranteed () const;
};

class Setup;
class Viewport;
class Pixels;
class ScreenMessage;

struct GlobalData {
   
   GlobalData ();
   ~GlobalData ();

   Setup* setup;
   Viewport* viewport;
   Pixels* pixels;
   ScreenMessage* messages;
   Rng* rng;
};

class Setup {
   public:
      Setup (int argc, char* argv[]);

      bool fullscreen, verbose, shadow;
      int resx, resy, bpp, pixels;

   private:
      struct ParseState {
         ParseState (int argc, char** argv) : argc(argc), argv(argv), iarg(1) {}

         int argc, iarg;
         char** argv;
      };

      void giveHelp (int status, string msg = "") const;
      bool popInt (int& val, ParseState& state) const;
};

class Viewport {
   public:
      Viewport (GlobalData* global);
      ~Viewport ();

      SDL_Surface* surface;
      int resx, resy, bpp;
      bool fullscreen, doublebuffered, hardware, shadow;
      inline void lock ();
      inline void unlock ();
      void clear ();
      Uint32 mapColor (Uint8 r, Uint8 g, Uint8 b) const;
      inline void putpixel (int x, int y, Uint32 color);
      void flip ();

   private:
      SDL_Surface* real_surface;
      int bytesPerPixel, pitch;
      bool verbose;
      GlobalData* global;
};

struct Event {
   enum Type {None, Left, Right, Up, Down, Quit, Freeze, Dump, SpeedUp, SpeedDown,
      ChangeRng};
   Type type;

   Event (const Type t) : type(t) {}
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

class Pixels {
   public:
      Pixels (GlobalData*);
      void tick ();
      void render ();
      void receiveEvent (const Event& e);

   private:
      void rinit ();

      struct Pixel {
         int x, y;
         Uint32 color;

         Pixel () : x(0), y(0), color(0) {}
         Pixel (int x, int y, Uint32 color) : x(x), y(y), color(color) {}
      };

      typedef vector<Pixel> TPixels;
      TPixels pixels;
      int stepn, steps, stepw, stepe, speed;
      bool frozen;
      GlobalData* global;
      Viewport* viewport;
      Rng* rng;
};

class ScreenMessage {
   public:
      ScreenMessage (GlobalData* global, int n, int x, int y, int r,
         int g, int b, int fade=60, int live=120);
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


GlobalData::GlobalData () : setup(0), viewport(0), messages(0), rng(0) {}

GlobalData::~GlobalData () {
   if (setup) delete setup;
   if (viewport) delete viewport;
   if (messages) delete messages;
   if (rng) delete rng;
}

void Setup::giveHelp (int status, string msg) const {
   if (msg != "") cout << "ERROR: " << msg << "\n";
   cout << "usage: pixels [arguments]\n\n";
   cout << "valid arguments are:\n";
   cout << "   -h, --help               : show this help screen\n";
   cout << "   -f, --fullscreen         : start fullscreen\n";
   cout << "   -r, --res x y            : resolution\n";
   cout << "   --shadow                 : use shadow buffer\n";
   cout << "   -n, --pixels n           : number of pixels\n";
   exit (status);
}

bool Setup::popInt (int& val, Setup::ParseState& state) const {
   if (state.iarg >= state.argc-1) return false;
   string arg (state.argv[++state.iarg]);
   stringstream ss (arg);
   ss >> val;
   if (ss.fail ()) return false;
   if (!ss.eof ()) return false;
   return true;
}

Setup::Setup (int argc, char* argv[]) : fullscreen(false), resx(640),
   resy(480), bpp(32), verbose(true), shadow(false), pixels(100)
{
   ParseState state (argc, argv);
   for (state.iarg = 1; state.iarg <= state.argc-1; state.iarg++) {
      string arg = state.argv[state.iarg];
      if (arg == "-h" || arg == "--help") giveHelp (0);
      else if (arg == "-f" || arg == "--fullscreen") fullscreen = true;
      else if (arg == "-r" || arg == "--res") {
         bool flag;
         flag = popInt (resx, state);
         if (flag) flag = popInt (resy, state);
         if (!flag) giveHelp (1, "integer expected");
         if (resx <= 0) giveHelp (1, "x must be positive definite");
         if (resy <= 0) giveHelp (1, "y must be positive definite");
      }
      else if (arg == "--shadow") shadow = true;
      else if (arg == "-n" || arg == "--pixels") {
         if (!popInt (pixels, state)) giveHelp (1, "integer expected");
         if (pixels <= 0) giveHelp (1, "n must be positive definite");
      }
      else giveHelp (1, "unknown argument '" + arg + "'");
   }
}

Viewport::Viewport (GlobalData* global) : resx(global->setup->resx),
   resy(global->setup->resy), verbose(global->setup->verbose),
   shadow(global->setup->shadow), global(global)
{
   if (SDL_Init (SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0)
      throw EFatal ("SDL init failed");

   Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF |
      (global->setup->fullscreen ? SDL_FULLSCREEN : 0);
   bpp = SDL_VideoModeOK (resx, resy, global->setup->bpp, flags);
   if (!bpp) throw EFatal ("requested resolution not available");
   real_surface = SDL_SetVideoMode (resx, resy, bpp, flags);
   if (!real_surface) throw EFatal ("unable to setup video");

   if (shadow) {
      SDL_PixelFormat* f = real_surface->format;
      surface = SDL_CreateRGBSurface (SDL_HWSURFACE, resx, resy, bpp,
         f->Rmask, f->Gmask, f->Bmask, f->Amask);
      if (!surface) throw EFatal ("unable to create shadow surface");
   }
   else surface = real_surface;
   bytesPerPixel = surface->format->BytesPerPixel;
   pitch = surface->pitch;
   clear ();

   fullscreen = real_surface->flags && SDL_FULLSCREEN;
   doublebuffered = real_surface->flags && SDL_DOUBLEBUF;
   hardware = real_surface->flags && SDL_HWSURFACE;

   if (verbose) {
      cout << "created a " << resx << "x" << resy << "@" << bpp << " surface\n";
      if (hardware) cout << "managed to obtain a hardware surface\n";
      if (doublebuffered) cout << "surface is doublebuffered\n";
      if (shadow) cout << "allocated a shadow buffer\n";
   }

   gfxPrimitivesSetFont (0, 0, 0);
}

void Viewport::flip () {
   if (shadow) SDL_BlitSurface (surface, 0, real_surface, 0);
   SDL_Flip (real_surface);
}

Viewport::~Viewport () {
   if (shadow) SDL_FreeSurface (surface);
   SDL_Quit ();
}

void Viewport::lock () {
   if (SDL_MUSTLOCK(surface)) SDL_LockSurface (surface);
}

void Viewport::unlock () {
   if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface (surface);
}

void Viewport::clear () {
   lock ();
   SDL_FillRect (surface, 0, mapColor (0, 0, 0));
   unlock ();
}

Uint32 Viewport::mapColor (Uint8 r, Uint8 g, Uint8 b) const {
   return SDL_MapRGB (surface->format, r, g, b);
}

void Viewport::putpixel (int x, int y, Uint32 color) {
   switch (bytesPerPixel) {
      case 4:
         static_cast<Uint32*>(surface->pixels)[x+y*pitch/4] = color;
         break;
      case 3:
         int idx;
         idx = x*3+y*pitch;
         static_cast<Uint8*>(surface->pixels)[idx]   =
            *reinterpret_cast<Uint8*>(&color);
         static_cast<Uint8*>(surface->pixels)[idx+1] =
            *(reinterpret_cast<Uint8*>(&color) + 1);
         static_cast<Uint8*>(surface->pixels)[idx+2] =
            *(reinterpret_cast<Uint8*>(&color) + 2);
         break;
      case 2:
         static_cast<Uint16*>(surface->pixels)[x+y*pitch/2] = color;
         break;
      case 1:
         static_cast<Uint8*>(surface->pixels)[x+y*pitch] = color;
         break;
      default:
         stringstream ss;
         ss << " unhandled bytes per pixel " << bytesPerPixel;
         throw EFatal (ss.str ());
   }
}

Rng::Rng (const Type t) : type(t) {
   reset (t);
}

const string Rng::name () const {
   switch (type) {
      case Default:
         return "default";
      case MultiplyWithCarry:
         return "multiply with carry";
      case LC1:
         return "linear congruential 1 (numerical recepies)";
      case LC2:
         return "linear congruential 2 (RtlUniform)";
      default:
         throw EBug ("Rng::name: invalid rng type");
   }
}

void Rng::reset (const Type t) {
   type = t;
   switch (type) {
      case Default:
         srand (time (0));
         max = RAND_MAX;
         break;
      case MultiplyWithCarry:
         srand (time (0));
         state[0] = rand ();
         state[1] = rand ();
         max = 0xFFFFFFFF;
         break;
      case LC1:
         srand (time (0));
         state[0] = rand ();
         max = 0xFFFFFFFF;
         break;
      case LC2:
         srand (time (0));
         state[0] = rand ();
         max = 0x0FFFFFFF - 1;
         break;
      default:
         throw EBug ("Rng::reset: invalid rng type");
   }
   for (int i = 0; i <= 31; i++) {
      if ((1 << i) >= max) {
         bits = i+1;
         break;
      }
   }
}

void Rng::cycle () {
   switch (type) {
      case Default:
         reset (MultiplyWithCarry);
         break;
      case MultiplyWithCarry:
         reset (LC1);
         break;
      case LC1:
         reset (LC2);
         break;
      case LC2:
         reset (Default);
         break;
      default:
         throw EBug ("Rng::cycle: invalid rng type");
   }
}

inline const Uint32 Rng::rnd () {
   switch (type) {
      case Default:
         return rand ();
         break;
      case MultiplyWithCarry:
         state[0] = 36969 * (state[0] & 65535) + (state[1] >> 16);
         state[1] = 18000 * (state[1] & 65535) + (state[1] >> 16);
         return (state[0] << 16) + state[1];
         break;
      case LC1:
         return (state[0] = 1664525 * state[0] + 1013904223);
         break;
      case LC2:
         return (state[0] = (2147483629 * state[0] + 2147483587) % 0x0FFFFFFF);
         break;
      default:
         throw EBug ("Rng::rnd: invalid rng type");
   }
}

Pixels::Pixels (GlobalData* global)
:
   global(global), viewport(global->viewport),
   pixels(TPixels(global->setup->pixels)), stepn(1), steps(1),
   stepw(1), stepe(1), frozen(false), speed(4), rng(global->rng)
{
   for (int i = 0; i <= global->setup->pixels-1; i++) pixels[i] = Pixel(
      rng->rnd () % viewport->resx,
      rng->rnd () % viewport->resy,
      viewport->mapColor (rng->rnd () % 256, rng->rnd () % 256, rng->rnd () % 256)
   );
}

void Pixels::render () {
   viewport->lock ();
   for (TPixels::const_iterator i = pixels.begin (); i != pixels.end (); i++)
      viewport->putpixel (i->x, i->y, i->color);
   viewport->unlock ();
}

void Pixels::tick () {
   Uint32 rval;
   if (frozen) return;
   int shift = rng->bits - 1;
   int mshift = rng->bits - 2;
   for (TPixels::iterator i = pixels.begin (); i != pixels.end (); i++) {
      for (int j = 1; j <= speed; j++) {
         if (shift > mshift) {
            rval = rng->rnd ();
            shift = 0;
         }
         switch ((rval >> shift) & 3) {
            case 0:
               if ((i->x += stepe) >= viewport->resx) i->x -= stepe;
               break;
            case 1:
               if ((i->x -= stepw) < 0) i->x += stepw;
               break;
            case 2:
               if ((i->y += steps) >= viewport->resy) i->y -= steps;
               break;
            case 3:
               if ((i->y -= stepn) < 0) i->y += stepn;
               break;
         }
         shift += 2;
      }
   }
}

void Pixels::receiveEvent (const Event& evt) {
   stringstream ss;
   switch (evt.type) {
      case Event::Left:
         if (stepe > 1) {
            stepe--;
            ss << "step width east: " << stepe;
            global->messages->pushMessage (ss.str ());
         } else {
            stepw++;
            ss << "step width west: " << stepw;
            global->messages->pushMessage (ss.str ());
         }
         break;
      case Event::Right:
         if (stepw > 1) {
            stepw--; 
            ss << "step width west: " << stepw;
            global->messages->pushMessage (ss.str ());
         } else {
            stepe++;
            ss << "step width east: " << stepe;
            global->messages->pushMessage (ss.str ());
         }
         break;
      case Event::Up:
         if (steps > 1) {
            steps--;
            ss << "step width south: " << steps;
            global->messages->pushMessage (ss.str ());
         } else {
            stepn++;
            ss << "step width north: " << stepn;
            global->messages->pushMessage (ss.str ());
         }
         break;
      case Event::Down:
         if (stepn > 1) {
            stepn--;
            ss << "step width north: " << stepn;
            global->messages->pushMessage (ss.str ());
         } else {
            steps++;
            ss << "step width south: " << steps;
            global->messages->pushMessage (ss.str ());
         }
         break;
      case Event::SpeedUp:
         speed++;
         ss << "speed: " << speed;
         global->messages->pushMessage (ss.str ());
         break;
      case Event::SpeedDown:
         if (speed > 1) speed--;
         ss << "speed: " << speed;
         global->messages->pushMessage (ss.str ());
         break;
      case Event::Freeze:
         if (frozen) global->messages->pushMessage ("unfrozen");
         else global->messages->pushMessage ("frozen");
         frozen = !frozen;
         break;
      case Event::ChangeRng:
         rng->cycle ();
         global->messages->pushMessage ("rng set to " + rng->name ());
         break;
      case Event::Dump:
         ss <<    "north: " << stepn << "   west: " << stepw;
         ss << "   south: " << steps << "   east: " << stepe;
         ss << "   speed: " << speed;
         global->messages->pushMessage (ss.str ());
         global->messages->pushMessage ("random number generator: " + rng-> name ());
         break;
   }
}

ScreenMessage::ScreenMessage (GlobalData* global, int n, int x, int y, int r,
   int g, int b, int fade, int live)
:
   global(global), n(n), x(x), y(y), r(r), g(g), b(b), fade(fade), live(live)
{}

void ScreenMessage::tick () {
   if (queue.empty ()) return;
   for (TQueue::iterator i = queue.begin (); i != queue.end (); i++) {
      i->clock++;
   }
   while (!queue.empty () && queue.front ().clock > 2*fade+live)
      queue.pop_front ();
}

void ScreenMessage::clear () {
   queue.clear ();
}

void ScreenMessage::pushMessage (const string& msg) {
   if (queue.size () >= n) queue.pop_front ();
   queue.push_back (Message(msg));
}

void ScreenMessage::render () {
   int ctr(0);
   Uint8 alpha;
   for (TQueue::const_iterator i = queue.begin (); i != queue.end (); i++) {
      if (i->clock <= fade) alpha = (255 * i->clock) / fade;
      if (i->clock > fade && i->clock <= fade + live) alpha = 255;
      if (i->clock > fade + live && i->clock <= 2*fade+live)
         alpha = (255 * (2*fade+live - i->clock)) / fade;
      stringRGBA (global->viewport->surface, x, y + 10*ctr++,
         i->content.c_str (), r, g, b, alpha);
   }
}

EventManager::EventManager (int delay, int repeat) :
   delay(delay), repeat(repeat) {}

bool EventManager::pollEvent (Event& evt) {
   if (queue.empty ()) return false;
   evt = queue.front ();
   queue.pop_front ();
   return true;
}

void EventManager::tick () {
   SDL_Event evt;
   while (SDL_PollEvent (&evt)) {
      bool valid;
      Event::Type t;
      switch (evt.type) {
         case SDL_QUIT:
            queue.push_back (Event (Event::Quit));
            break;
         case SDL_KEYDOWN:
         case SDL_KEYUP:
            valid = true;
            switch (evt.key.keysym.sym) {
               case SDLK_q:
                  t = Event::Quit;
                  break;
               case SDLK_UP:
                  t = Event::Up;
                  break;
               case SDLK_DOWN:
                  t = Event::Down;
                  break;
               case SDLK_LEFT:
                  t = Event::Left;
                  break;
               case SDLK_RIGHT:
                  t = Event::Right;
                  break;
               case (SDLK_f):
                  t = Event::Freeze;
                  break;
               case (SDLK_d):
                  t = Event::Dump;
                  break;
               case (SDLK_PLUS):
                  t = Event::SpeedUp;
                  break;
               case (SDLK_MINUS):
                  t = Event::SpeedDown;
                  break;
               case (SDLK_r):
                  t = Event::ChangeRng;
                  break;
               default:
                  valid = false;
            }
            if (valid) {
               if (evt.key.type == SDL_KEYDOWN) {
                  queue.push_back (Event (t));
                  repeaters[t] = 0;
               } else {
                  repeaters.erase (t);
               }
            }
            break;
         default:
            break;
      }
  }
  for (TRepeaters::iterator i = repeaters.begin (); i != repeaters.end (); i++) {
      if (i->second >= delay && ((i->second - delay) % repeat == 0))
         queue.push_back (i->first);
      i->second++;
  }
}

int main (int argc, char* argv[]) {
   GlobalData* global = new GlobalData;
   try {
      global->setup = new Setup(argc, argv);
      global->viewport = new Viewport (global);
      global->rng = new Rng;
      global->pixels = new Pixels(global);
      global->messages = new ScreenMessage (global, 5, 5, 5, 255, 255, 255);
      global->pixels->render ();
      global->messages->render ();
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
            global->pixels->receiveEvent (evt);
         }

         global->pixels->tick ();
         global->messages->tick ();

         if (frame++ == 2) {
            frame = 0;
            global->viewport->clear ();
            global->pixels->render ();
            global->messages->render ();
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
