#include <vector>
#include <list>
#include <sstream>
#include <thread>
#include <random>

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
using namespace CryptoPP;
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
    setenv("PATH", "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin", 1);
    try {
        ScoringManager mgr(token);

        Log() << "View scoring report: " << IMSC_URL << "/session/" << token << "/report";
        Log() << "Stop scoring: " << IMSC_URL << "/session/" << token << "/stop";
        Log() << "Restart scoring: " << IMSC_URL << "/session/" << token << "/restart";

        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::normal_distribution<> distr(10, 30); // define the range

        Status s;
        while(1) {
            s = mgr.status();
            switch (s) {
                case Status::Termination:
                    Log() << "Terminating";
                    return 0;
                case Status::Wait:
                    Log() << "Sleeping...";
                    std::this_thread::sleep_for(std::chrono::seconds(int(distr(gen))));
                    break;
                case Status::Score:
                    Log() << "Scoring...";
                    mgr.score();
                    break;
                case Status::Invalid:
                    Err() << "Unrecognized status";
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
    return Status::Invalid;
}

void ScoringManager::notify(const string& msg) {
    // Assuming the user has UID 1000
    system(("sudo -u $(id -nu 1000) DISPLAY=:0 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus "
            "notify-send 'Scoring Engine' '" + msg + "'").c_str());
    Notif() << msg;
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
    byte key_digest[SHA256::DIGESTSIZE];
    SHA256().CalculateDigest(key_digest, (byte*) key.data(), key.size());

    CFB_Mode<AES>::Encryption
        encryption((byte*)key_digest, SHA256::DIGESTSIZE, iv);
    StringSource encryptor(plaintext, true,
                           new StreamTransformationFilter(encryption,
                                                          new StringSink(ciphertext)));
    string encoded;
    StringSource encoder(iv_str + ciphertext, true,
                         new Base64Encoder(
                             new StringSink(encoded)));
    return encoded;
}

std::string decrypt(const string& encoded) {
    string plaintext;
    string decoded = "";

    StringSource
        decoder(encoded, true, new Base64Decoder(new StringSink(decoded)));

    string iv = decoded.substr(0, 16);
    string ciphertext = decoded.substr(16);

    string key = (char*) IMSC_SECRET;
    byte key_digest[SHA256::DIGESTSIZE];
    SHA256().CalculateDigest(key_digest, (byte*) key.data(), key.size());

    CFB_Mode<AES>::Decryption decryption(key_digest,
                                         SHA256::DIGESTSIZE,
                                         (byte*)iv.c_str());

    StringSource decryptor(ciphertext, true,
                           new StreamTransformationFilter(decryption,
                                                          new StringSink(plaintext)));
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
