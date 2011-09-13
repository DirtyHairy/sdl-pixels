#ifndef UNIQUE_H
#define UNIQUE_H

using namespace std;

class Unique {
   public:
      typedef unsigned long TId;
      Unique ();
      ~Unique ();
      const TId getId () const;

   private:
      TId id;

      Unique (const Unique&);
      Unique& operator= (const Unique&);
};

#endif // UNIQUE_H
