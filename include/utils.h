#pragma once
#ifndef IMSC_UTILS_H
#define IMSC_UTILS_H

#include <ctime>
#include <string>
#include <iosfwd>

time_t str_to_rawtime(const std::string& dt);

// get current time struct
tm* get_time();

// get formatted time
std::string get_time_str();

class Logger {

    public:

        enum MessageType {
            NOTIFICATION,
            INFO,
            ERROR
        };

    private:

        static std::ofstream ofs;
        MessageType type;

    public:

        Logger(const std::string& pfx, MessageType mt);
        ~Logger();
        Logger& operator<<(const std::string& str);
        Logger& operator<<(int n);

};

#define Log() Logger("I: ", Logger::INFO)
#define Err() Logger("E: ", Logger::ERROR)
#define Notif() Logger("N: ", Logger::NOTIFICATION)

#endif
