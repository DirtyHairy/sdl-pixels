#include <iostream>
#include <cstdlib>
#include <sstream>

#include "setup.h"

using namespace std;

void Setup::giveHelp (int status, string msg) const {
   if (msg != "") cout << "ERROR: " << msg << "\n";
   cout << "usage: pixels [arguments]\n\n";
   cout << "valid arguments are:\n";
   cout << "   -h, --help               : show this help screen\n";
   cout << "   -f, --fullscreen         : start fullscreen\n";
   cout << "   -r, --res x y            : resolution\n";
   cout << "   --shadow                 : use shadow buffer\n";
   cout << "   -n, --pixels n           : number of pixels\n";
   cout << "   -m, --messages n         : message queue length\n";
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
   resy(480), bpp(32), verbose(true), shadow(false), pixels(100), messages(5)
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
      else if (arg == "-m" || arg == "--messages") {
         if (!popInt (messages, state)) giveHelp (1, "integer expected");
         if (messages <= 1) giveHelp (1, "positive integer expected");
      }
      else giveHelp (1, "unknown argument '" + arg + "'");
   }
   if (messages > resy / 10) {
      messages = resy / 10;
      cerr << "WARNING: message queue length reduced to " << messages << ".\n";
   }
}
