#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

using namespace std;

class EPixelsExcpt: public exception {
   public:
      EPixelsExcpt (const string& msg);
      ~EPixelsExcpt () throw ();
      const char* what () const throw ();

   protected:
      string msg;
};

class EFatal: public EPixelsExcpt {
   public:
      EFatal (const string& msg);
};

class EBug: public EPixelsExcpt {
   public:
      EBug (const string& msg);
};

#endif // EXCEPTIONS_H
