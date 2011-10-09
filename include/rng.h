#ifndef RNG_H
#define RNG_H

#include <string>
#include <SDL.h>

#include "exceptions.h"

using namespace std;

class Rng {
   public:
      enum Type {Default, MultiplyWithCarry, LC1, LC2};

      Rng (Type t=Default);
      string name () const;
      inline Uint32 rnd ();
      inline float rnd_unit ();
      void reset (Type t);
      void cycle ();

      int bits;
      Uint32 max;

   private:
      Uint32 state[2];
      Type type;
};

inline Uint32 Rng::rnd () {
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
         return (state[0] = (2147483629 * state[0] + 2147483587) % 0x3FFFFFFF);
         break;
      default:
         throw EBug ("Rng::rnd: invalid rng type");
   }
}

inline float Rng::rnd_unit () {
   return float (rnd ()) / max;
}

#endif // RNG_H
