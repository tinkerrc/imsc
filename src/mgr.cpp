#include <vector>
#include <list>
#include <sstream>
#include <thread>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <cryptopp/osrng.h>

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>
#include "nlohmann/json.hpp"

#include "mgr.h"
#include "config.h"
#include "utils.h"

using std::string;
using std::vector;
using namespace CryptoPP; // source file so it's OK
using json = nlohmann::json;

json GET(const string& url);
void POST(const string& url, const json& data);

ScoringManager::ScoringManager(const string& tok)
: token(tok) {
    Log() << "Acquiring image information";
    json init_data = GET(IMSC_URL + ("/session/" + token));

    Log() << "Setting up image";
    start_time = init_data["start_time"]; // YYYY-MM-DD HH:MM:SS
    image_name = init_data["image_name"];
    duration = init_data["duration"];

    Log() << "Loading rules";
    Checklist cl;
    json jcl = init_data["checklist"];
    for (auto it = jcl.begin(); it != jcl.end(); ++it) {
        Rule r;
        r.id = (*it)["id"];
        r.name = (*it)["rule_name"];
        r.pts = (*it)["points"];
        r.cmd = (*it)["command"];
        r.code = (*it)["exit_code"];
        cl.add_rule(r);
    }
    checklist = cl;

    Log() << "Initialization done";
}

int ScoringManager::session(const std::string& token) {
    try {
        ScoringManager mgr(token);
        Log() << "View scoring report: " << IMSC_URL << "/session/" << token << "/report";
        Log() << "Stop scoring: " << IMSC_URL << "/session/" << token << "/stop";

        Status s;
        while(1) {
            s = mgr.status();
            switch (s) {
                case Status::Termination:
                    Log() << "Terminating";
                    return 0;
                case Status::Wait:
                    Log() << "Sleeping...";
                    std::this_thread::sleep_for(std::chrono::seconds(15));
                    break;
                case Status::Score:
                    Log() << "Scoring...";
                    mgr.score();
                    break;
                case Status::Invalid:
                    return 1;
            }
        }
    } catch (const std::exception& e) {
        Err() << e.what();
        notify("Scoring engine crashed.");
        Err() << "Terminating.";
        return 1;
    }
}

void ScoringManager::score() {
    try {
        ScoringReport report = checklist.check();
        Log() << "Current score: " << report.pts();
        Log() << "Uploading scoring report";
        POST(IMSC_URL + string("/session/") + token + "/report", report);
        Log() << "Done";

        if (report.gained_since(last_report)) notify("You gained points!");
        if (report.lost_since(last_report)) notify("You lost points!");
        last_report = report;
    } catch (const std::runtime_error &e) {
        Err() << e.what();
        Err() << "Failed to score";
        notify("Error: failed to score");
        return;
    }
}

Status ScoringManager::status() {
    Log() << "Fetching status";
    string stat = GET(IMSC_URL + ("/session/" + token + "/status"));
    if (stat == "Wait") return Status::Wait;
    if (stat == "Score") return Status::Score;
    if (stat == "Termination") return Status::Termination;
    Err() << "Unrecognized status: " << stat;
    return Status::Invalid;
}

void ScoringManager::notify(const string& msg) {
    Notif() << msg;
    system(("DISPLAY=:0 /usr/bin/notify-send 'Scoring Engine' '" + msg + "'").c_str());
}

// file-local functions

string encrypt(const string& plaintext) {
    // Generate a random IV
    AutoSeededRandomPool rnd;
    SecByteBlock iv(AES::BLOCKSIZE);
    rnd.GenerateBlock(iv, iv.size());
    string iv_str = string((char*)iv.BytePtr(), iv.size());
    assert(iv_str.size() == 16);
    assert(iv.size() == 16);

    string ciphertext;
    string key = (char*) IMSC_SECRET;
    byte key_digest[CryptoPP::SHA256::DIGESTSIZE];
    CryptoPP::SHA256().CalculateDigest(key_digest, (byte*) key.data(), key.size());

    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption
        encryption((byte*)key_digest, CryptoPP::SHA256::DIGESTSIZE, iv);
    CryptoPP::StringSource encryptor(plaintext, true,
                                     new CryptoPP::StreamTransformationFilter(encryption,
                                                                              new CryptoPP::StringSink(ciphertext)));
    string encoded;
    CryptoPP::StringSource encoder(iv_str + ciphertext, true,
                                   new CryptoPP::Base64Encoder(
                                       new StringSink(encoded)));
    return encoded;
}

std::string decrypt(const string& encoded) {
    string plaintext;
    string decoded = "";

    CryptoPP::StringSource
        decoder(encoded, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded)));

    string iv = decoded.substr(0, 16);
    string ciphertext = decoded.substr(16);

    string key = (char*) IMSC_SECRET;
    byte key_digest[CryptoPP::SHA256::DIGESTSIZE];
    CryptoPP::SHA256().CalculateDigest(key_digest, (byte*) key.data(), key.size());

    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryption(key_digest,
                                                             CryptoPP::SHA256::DIGESTSIZE,
                                                             (byte*)iv.c_str());

    CryptoPP::StringSource decryptor(ciphertext, true,
                                     new CryptoPP::StreamTransformationFilter(decryption,
                                                                              new CryptoPP::StringSink(plaintext)));
    return plaintext;
}

json GET(const string& url) {
    std::stringstream res;
    curlpp::Easy req;

    req.setOpt(new curlpp::options::Url(url));
    req.setOpt(new curlpp::options::WriteStream(&res));
    req.perform();

    json j = json::parse(res.str());
    int code = curlpp::infos::ResponseCode::get(req);
    if (!j["success"].get<bool>())
        throw std::runtime_error(
            "Failed to fetch data from server (" + std::to_string(code) + ")"
        );

    return json::parse(decrypt(j["message"]));
}

void POST(const string& url, const json& data) {
    std::stringstream res;
    curlpp::Easy req;

    string ciphertext = encrypt(data.dump());

    req.setOpt(new curlpp::options::Url(url));
    req.setOpt(new curlpp::options::WriteStream(&res));
    std::list<string> header;
    header.push_back("Content-Type: text/plain");

    req.setOpt(new curlpp::options::HttpHeader(header));

    req.setOpt(new curlpp::options::PostFields(ciphertext));
    req.setOpt(new curlpp::options::PostFieldSize(ciphertext.size()));
    req.setOpt(new curlpp::options::FollowLocation(true));

    req.perform();

    int code = curlpp::infos::ResponseCode::get(req);
    if (code >= 400)
        throw std::runtime_error(
            "Failed to upload data to server (" + std::to_string(code) + ")"
        );
}
