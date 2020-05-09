#include "client.h"

#include <curlpp/Easy.hpp>

using std::string;

Client::Client(const string& url) : url(url) {
    if (created) {
        // because of curlpp
        throw std::logic_error("only one client object is allowed per process");
    }
    created = true;
}

bool Client::verify_token(const string& token) {
    return true;
}

void Client::set_token(const string& token) {
    // TODO: confirm token
}

bool Client::send(ReportType ty, const string& cipher) {
    return false;
}

bool Client::created = false;
