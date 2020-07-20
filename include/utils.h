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

class Log {
    private:
        static std::ofstream ofs;

    public:
        Log(const std::string& pfx = "I: ");
        ~Log();
        Log& operator<<(const std::string& str);
        Log& operator<<(int n);
};

#define Err() Log("E: ")
#define Notif() Log("N: ")

#endif
