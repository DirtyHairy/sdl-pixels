#include <string>
#include "rng.h"

#include "exceptions.h"

using namespace std;

Rng::Rng (Type t) : type(t) {
   reset (t);
}

string Rng::name () const {
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

void Rng::reset (Type t) {
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
         max = 0x3FFFFFFF - 1;
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
