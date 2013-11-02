#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
    public:
        Logger();

        virtual void Log(const std::string& msg) = 0;

    private:
        Logger(const Logger&);

        const Logger& operator=(const Logger&);
};

#endif // LOGGER_H
