#include "utils.h"
#include "config.h"

#include <fstream>
#include <iostream>
#include <ctime>

using std::string;

time_t str_to_rawtime(const string& dt) {
    struct tm t;
    strptime(dt.c_str(), "%Y-%m-%d %H:%M:%S", &t);
    return mktime(&t);
}

tm* get_time() {
    time_t rawtime;
    tm* ltime;
    time(&rawtime);
    ltime = localtime(&rawtime);
    return ltime;
}

string get_time_str() {
    char buffer[80];
    strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",get_time());
    return string(buffer);
}

Logger::Logger(const string& pfx, MessageType mt) : type(mt) {
    string msg = "[" + get_time_str() + "]  " + pfx;
    if (type != Logger::ERROR)
        std::cout << msg;
    else
        std::cerr << msg;
    if (ofs.good())
        ofs << msg;
}

Logger::~Logger() {
    if (type != Logger::ERROR)
        std::cout << '\n';
    else
        std::cerr << '\n';
    if (ofs.good())
        ofs << '\n';
}

Logger& Logger::operator<<(const std::string& str) {
    if (type != Logger::ERROR)
        std::cout << str;
    else
        std::cerr << str;
    if (ofs.good())
        ofs << str;
    return *this;
}

Logger& Logger::operator<<(int n) {
    if (type != Logger::ERROR)
        std::cout << n;
    else
        std::cerr << n;
    if (ofs.good())
        ofs << n;
    return *this;
}

std::ofstream Logger::ofs = std::ofstream(IMSC_LOG_FILE, std::ofstream::out | std::ofstream::app);
