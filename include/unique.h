#ifndef UNIQUE_H
#define UNIQUE_H

using namespace std;

class Unique {
   public:
      typedef unsigned long TId;

      Unique ();
      Unique (const Unique&);
      ~Unique ();
      const TId getId () const;
      Unique& operator= (const Unique&);

   private:
      TId id;
      void newId ();
};

#endif // UNIQUE_H
