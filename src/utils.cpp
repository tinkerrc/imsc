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

Log::Log(const string& str) {
    string msg = "[" + get_time_str() + "]  " + str;
    std::cout << msg;
    if (ofs.good())
        ofs << msg;
}

Log::~Log() {
    std::cout << '\n';
    if (ofs.good())
        ofs << '\n';
}

Log& Log::operator<<(const std::string& str) {
    std::cout << str;
    if (ofs.good())
        ofs << str;
    return *this;
}
std::ofstream Log::ofs = std::ofstream(LOG_FILE, std::ofstream::out | std::ofstream::app);
