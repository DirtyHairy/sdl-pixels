#include <iostream>
#include <cstdlib>
#include <sstream>

#include "setup.h"
#include "logger.h"

using namespace std;

namespace {

   struct ParseState {
       ParseState (int argc, char** argv) : argc(argc), argv(argv), iarg(1) {}

       int argc, iarg;
       char** argv;
   };

   void giveHelp (Logger& logger, int status, string msg = "") {
       if (msg != "") logger.Log("ERROR: " + msg);
       logger.Log("usage: pixels [arguments]\n");
       logger.Log("valid arguments are:");
       logger.Log("   -h, --help               : show this help screen");
       logger.Log("   -f, --fullscreen         : start fullscreen");
       logger.Log("   -r, --res x y            : resolution");
       logger.Log("   --shadow                 : use shadow buffer");
       logger.Log("   -n, --pixels n           : number of pixels");
       logger.Log("   -m, --messages n         : message queue length");
       exit (status);
   }

   bool popInt (int& val, ParseState& state) {
       if (state.iarg >= state.argc-1) return false;
       string arg (state.argv[++state.iarg]);
       stringstream ss (arg);
       ss >> val;
       if (ss.fail ()) return false;
       if (!ss.eof ()) return false;
       return true;
   }

}

void Setup::ResetToDefaults () {
    fullscreen = false;
    resx = 640;
    resy = 480;
    bpp = 32;
    verbose = true;
    shadow = false;
    pixels = 1000;
    messages = 5;
}

Setup:: Setup () {
    ResetToDefaults();
}

Setup::Setup (Logger& logger, int argc, char* argv[]) {
    stringstream ss;

    ResetToDefaults();

    ParseState state (argc, argv);
    for (state.iarg = 1; state.iarg <= state.argc-1; state.iarg++) {
        string arg = state.argv[state.iarg];
        if (arg == "-h" || arg == "--help") giveHelp (logger, 0);
        else if (arg == "-f" || arg == "--fullscreen") fullscreen = true;
        else if (arg == "-r" || arg == "--res") {
            bool flag;
            flag = popInt (resx, state);
            if (flag) flag = popInt (resy, state);
            if (!flag) giveHelp (logger, 1, "integer expected");
            if (resx <= 0) giveHelp (logger, 1, "x must be positive definite");
            if (resy <= 0) giveHelp (logger, 1, "y must be positive definite");
        }
        else if (arg == "--shadow") shadow = true;
        else if (arg == "-n" || arg == "--pixels") {
            if (!popInt (pixels, state)) giveHelp (logger, 1, "integer expected");
            if (pixels <= 0) giveHelp (logger, 1, "n must be positive definite");
        }
        else if (arg == "-m" || arg == "--messages") {
            if (!popInt (messages, state)) giveHelp (logger, 1, "integer expected");
            if (messages <= 1) giveHelp (logger, 1, "positive integer expected");
        }
        else giveHelp (logger, 1, "unknown argument '" + arg + "'");
    }
    if (messages > resy / 10) {
        messages = resy / 10;
        ss.str("");
        ss << "WARNING: message queue length reduced to " << messages << ".";
        logger.Log(ss.str());
    }
}
