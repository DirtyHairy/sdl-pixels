#ifndef MAIN_H
#define MAIN_H

#include "logger.h"
#include "setup.h"

class Main {
    public:
        Main(Logger& logger, Setup& setup);

        int Dispatch();

    private:
        Logger& logger;
        Setup& setup;
};

#endif // MAIN_H
