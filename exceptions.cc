#include "exceptions.h"

using namespace std;

// EPixelsExcpt

EPixelsExcpt::EPixelsExcpt (const string& msg) : msg(msg) {}

EPixelsExcpt::~EPixelsExcpt () throw () {}

const char* EPixelsExcpt::what () const throw () {
   return msg.c_str ();
}
// EFatal

EFatal::EFatal (const string& msg) : EPixelsExcpt(msg) {}

// EBug

EBug::EBug (const string& msg) : EPixelsExcpt(msg) {}
