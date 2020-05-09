#pragma once
#ifndef H_CLIENT
#define H_CLIENT

#include <string>
#include <stdexcept>

#include <curlpp/cURLpp.hpp>

namespace curlpp {
    class Cleanup;
}

enum class ReportType {
    InitialReport,
    Report,
    FinalReport
};

class InvalidTokenError : public std::runtime_error {
    InvalidTokenError(const std::string& s)
        : std::runtime_error(s) {}
};

class Client {
    private:
        curlpp::Cleanup cleanup;
        static bool created;
        std::string url;

    public:
        Client(const std::string& url);

        bool verify_token(const std::string& token);

        void set_token(const std::string& token);

        bool send(ReportType ty, const std::string& cipher);
};

#endif
