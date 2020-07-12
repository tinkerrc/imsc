#include "mgr.h"
#include "config.h"
#include "utils.h"

#include <vector>
#include <list>
#include <sstream>

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>
#include "toml11/toml.hpp"
#include "nlohmann/json.hpp"

using std::string;
using std::vector;
using json = nlohmann::json;

string GET(const string& url);
void POST(const string& url, const json& data);

ScoringManager::ScoringManager(const string& tok)
: token(tok) {

    Log() << "Acquiring image information";
#ifdef IMSC_MODE_SERVER
    auto init_data = json::parse(GET(IMSC_URL + ("/image/" + token)));
#else
#include "image_config"
    auto init_data = json::parse(json_str);
#endif

    Log() << "Setting up image";
    start_time = init_data["start_time"].get<string>(); // YYYY-MM-DD HH:MM:SS
    user = init_data["user"].get<string>();
    image_name = init_data["image_name"].get<string>();
    duration = init_data["duration"].get<int>();
    make_checklist(init_data["checklist"].get<string>());

#ifdef IMSC_MODE_SERVER
    Log() << "Writing desktop file for scoring report";
    string url = string(IMSC_URL) + "/report/" + token;
    string sr =
        "[Desktop Entry]\nEncoding=UTF-8\nName=Scoring Report\nType=Link\nURL="
        + url + "\nIcon=text-html\n";
    std::ofstream ofs("/home/" + user + "/Desktop/Scoring Report.desktop",
            std::ofstream::out | std::ofstream::trunc);

    if (ofs.good()) {
        ofs << sr;
        Log() << "Finished initialization";
    }
    else {
        Err() << "Error writing link to scoring report to desktop";
        Err() << "Use the URL " + url;
    }
#else
    Log() << "The scoring report will be written to the desktop soon";
#endif
}

void ScoringManager::score() {
    if (status() != Status::Score) {
        Log() << "Not yet...";
        return;
    }

    try {
        ScoringReport report = checklist.check();
        Log() << "Current score: " << report.pts();

        int mins = get_minutes_left();
        report.set_start_time(start_time);
        report.set_time_left(std::to_string(mins/60) + " hr " +
                std::to_string(mins%60) + " mins ");

#ifdef IMSC_MODE_LOCAL
        std::ofstream report_file(
                string("/home/") + user + "/Desktop/Scoring Report.txt",
                std::ofstream::out | std::ofstream::trunc);
        string report_data = report.to_string() + '\n';
        if (report_file.good()) {
            report_file << report_data;
        }
        else {
            Err() << "Failed to write scoring report, printing it here instead...\n" + report_data;
        }
#else
        POST(IMSC_URL + string("/report"), report);
#endif

        if (report.gained_since(last_report)) notify("You gained points!");
        if (report.lost_since(last_report)) notify("You lost points!");

        if (warn_mins >= get_minutes_left()) {
            notify("You have less than " + std::to_string(warn_mins) + "left.");
            warn_mins -= 5;
        }

        last_report = report;
    } catch (const std::runtime_error &e) {
        Err() << "failed to score";
        Err() << e.what();
        notify("Error: failed to score");
        return;
    }
}

Status ScoringManager::status() {
    Log() << "Fetching status";
    string stat = GET(IMSC_URL + ("/status/" + token));
    if (stat == "Wait") return Status::Wait;
    if (stat == "Score") return Status::Score;
    if (stat == "Termination") return Status::Termination;
    return Status::Invalid;
}

int ScoringManager::get_minutes_left() const {
    // NOTE: assuming time_t is Unix timestamp
    time_t term_time = str_to_rawtime(start_time) + duration * 60;
    return (term_time - time(0))/60;
}

void ScoringManager::notify(const string& msg) const {
    Log() << msg;
    system(("notify-send 'Scoring Engine' '" + msg + "'").c_str());
}

void ScoringManager::make_checklist(const string& chkls_str) {
    std::istringstream iss(chkls_str);
    auto toml_chkls = toml::parse(iss);

    Checklist chkls(image_name);
    auto rules = toml::find<toml::array>(toml_chkls, "rules");

    for (const auto& rule : rules) {
        Rule r;
        r.name = toml::find<string>(rule, "name");
        r.pts = toml::find_or<int>(rule, "pts", 1);
        r.uniq = toml::find_or<string>(rule, "uniq", "");
        r.cmd = toml::find<string>(rule, "cmd");
        r.code = toml::find_or<int>(rule, "code", 0);
        r.preset = toml::find_or<string>(rule, "preset", "");
        r.args = toml::find_or<vector<string>>(rule, "args", {});
        chkls.add_rule(r);
    }
    checklist = chkls;
}

string GET(const string& url) {
    std::stringstream res;
    curlpp::Easy req;
    try {
        req.setOpt(new curlpp::options::Url(url));
        req.setOpt(new curlpp::options::WriteStream(&res));
        req.perform();
    } catch (const curlpp::LogicError& e) {
        Err() << e.what();
    } catch (const curlpp::RuntimeError& e) {
        Err() << e.what();
    }

    int code = curlpp::infos::ResponseCode::get(req);
    if (code != 200)
        throw std::runtime_error(
                "failed to fetch data from server (" + std::to_string(code) + ")"
                );
    return res.str();
}

void POST(const string& url, const json& data) {
    std::stringstream res;
    curlpp::Easy req;
    try {
        req.setOpt(new curlpp::options::Url(url));
        req.setOpt(new curlpp::options::WriteStream(&res));
        std::list<string> header; 
        header.push_back("Content-Type: application/json"); 

        req.setOpt(new curlpp::options::HttpHeader(header)); 

        string dumped = data.dump();
        req.setOpt(new curlpp::options::PostFields(dumped));
        req.setOpt(new curlpp::options::PostFieldSize(dumped.size()));

        req.perform();
    } catch (const curlpp::LogicError& e) {
        Err() << e.what();
    } catch (const curlpp::RuntimeError& e) {
        Err() << e.what();
    }


    int code = curlpp::infos::ResponseCode::get(req);
    if (code != 200)
        throw std::runtime_error(
                "failed to send data to server (" + std::to_string(code) + ")"
                );
}
