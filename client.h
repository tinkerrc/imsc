#ifndef H_CLIENT
#define H_CLIENT

#include <string>
#include <stdexcept>

#include "config.h"

using std::string;

enum class ReportType {
    InitialReport,
    Report,
    FinalReport
};

class Client {
    public:
        Client(const string& url) {

        }

        bool verify_token(const string& token) {
            return true;
        }

        void set_token(const string& token) {
            // TODO: confirm token
        }

        bool send(ReportType ty, const string& cipher) {
            return false;
        }
};

#endif
