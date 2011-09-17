#include <deque>

#include "unique.h"
#include "exceptions.h"

using namespace std;

namespace {
   static Unique::TId nextId = 0;
   static const Unique::TId maxId = 0UL - 1UL;
   typedef deque<Unique::TId> TRecycleBin;
   static TRecycleBin recycle_bin;
}

// Unique

Unique::Unique () {
   if (recycle_bin.size () > 0) {
      id = recycle_bin.front ();
      recycle_bin.pop_front ();
   } else {
      if (nextId == maxId) throw EFatal("no free object identifiers left");
      id = nextId++;
   }
}

Unique::~Unique () {
   recycle_bin.push_back (id);
}

const Unique::TId Unique::getId () const {
   return id;
}

Unique& Unique::operator= (const Unique&) {
   return *this;
}
