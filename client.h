#ifndef H_CLIENT
#define H_CLIENT

#include <string>
#include <stdexcept>

#include <curlpp/Easy.hpp>
#include <curlpp/cURLpp.hpp>
#include <toml/parser.hpp>

#include "config.h"

using std::string;

enum class ReportType {
    InitialReport,
    Report,
    FinalReport
};

class InvalidTokenError : public std::runtime_error {
    InvalidTokenError(const string& s)
        : std::runtime_error(s) {}
};

class Client {
    private:
        curlpp::Cleanup cleanup;
        static bool created;
        string url = "";
    public:
        Client(const string& url) : url(url) {
            if (created) {
                // because of curlpp
                throw std::logic_error("only one client object is allowed per process");
            }
            created = true;
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

bool Client::created = false;

#endif
