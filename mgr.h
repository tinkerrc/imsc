#ifndef H_SCORING_MANAGER
#define H_SCORING_MANAGER

#include <string>
#include <map>
#include <ctime>
#include <fstream>
#include <iostream>

#include <toml.hpp>

#include "file.h"
#include "checklist.h"
#include "config.h" // SECRET...
#include "client.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::map;

class UnallowedActionError : public std::runtime_error {
    public:
        UnallowedActionError(const string& s)
            : std::runtime_error(s) {}
};

// the scoring engine
// the Checklist class actually scores
class ScoringManager {

    public:
        enum Status {
            Timed,
            Scoring,
            FinalReport,
            Grace,
            Termination
        };

        ScoringManager() {
            // read state information
            if (!read_data())
                throw std::runtime_error
                    ("ScoringManager(): Failed to read data. Contact proctor.");
            // record image launch time
            if (!has("launch_time"))
                set("launch_time", get_time_str());

            if (has("token"))
                client.set_token(get("token"));
        }

        ~ScoringManager() {
            save();
        }

        void log(const string& str) {
            string msg = "[" + get_time_str() + "]  " + str + "\n";
            std::cout << msg;
            if (logf.good())
                logf << msg;
        }

        /* initialize the image using token
         * 0 = success
         * 1 = already initialized
         * 2 = invalid token
         * 3 = not available yet
         * 4 = error
         */
        int init_img(const string &token) {
            if (has("init"))
                return 1;

            if (!reached_start_time()) {
                log("E: not the time to start yet");
                return 3;
            }

            set("init", "1");
            set("init_time", get_time_str());

            if (!client.verify_token(token))
                return 2;

            set("token", token);
            client.set_token(token);

            // report initial score to prevent early-start
            try {
                ScoringReport rep = make_checklist().check();
                add_time_info(rep);
                client.send(ReportType::InitialReport, encrypt(rep.to_string(true), secret));
                return 0;
            } catch (std::runtime_error& e) {
                log("E: init_img(): " + string(e.what()));
                log("E: failed to initialize image");
                vals.erase("init");
                vals.erase("init_time");
                return 4;
            }
        }

        // negative numbers mean minutes since scoring stopped
        int get_minutes_left() const {
            if (!has("init"))
                throw std::logic_error("get_minutes_left(): the image has not been initialized yet.");

            int dur = std::atoi(get("duration").c_str());
            int grace_time = std::atoi(get("grace_time").c_str());
            // NOTE: assuming time_t is Unix timestamp
            time_t term_time = str_to_rawtime(get("init_time")) + dur * 60;
            return (term_time - time(0))/60;
        }

        Status status() {
            if (has("init")) {
                if (get_minutes_left() > 0 && !has("stopped"))
                    return Status::Scoring;
                else if (has("stopped"))
                    return !has("final_report")? Status::FinalReport : Status::Termination;
                else if (std::atoi(get("grace_time").c_str()) >= -get_minutes_left())
                    return !has("final_report")? Status::FinalReport : Status::Grace;
                else // if cron broke?
                    return !has("final_report")? Status::FinalReport : Status::Termination;
            }
            return Status::Timed; // timed even if not initialized
        }

        void score() {
            if (!has("init")) {
                throw UnallowedActionError("score(): Image has not been initialized yet.");
            }

            if (has("last_scored_time") && !reached_scoring_interval()) {
                log("E: please wait til the scoring interval elapses");
                return;
            }

            // read last report from disk
            ScoringReport last_report;
            try {
                last_report = read_encrypted_file(LAST_REPORT_FILE, secret);
            } catch (...) {
                log("E: error reading last report");
            }

            // just in-case something went wrong
            if (status() == Status::Termination || status() == Status::Grace) {
                // regenerate the final report
                write_file(HOME_DIR "final-report", last_report.to_string(true));
                return;
            }

            try {
                ScoringReport rep = make_checklist().check();
                add_time_info(rep);

                if (status() == Status::FinalReport) {
                    log("I: stopped scoring");
                    log("I: writing final report to desktop");
                    string data = rep.to_string();
                    if (!write_file(HOME_DIR "/final-report.txt", data)) {
                        log("E: failed to write final report.");
                        log("I: FINAL REPORT: \n---====---" + rep.to_string() + "---====---\n");
                    }
                    client.send(ReportType::FinalReport, data);
                    set("final_report", "1");
                    stop_scoring();
                }
                else {
                    write_file(HOME_DIR "/report.txt", rep.to_string());
                }

                if (!write_encrypted_file(LAST_REPORT_FILE, rep.data(), secret)) {
                    log("E: failed to record current report");
                }
                set("last_scored_time", get_time_str());
                if (rep.gained_since(last_report)) notify("You gained points!");
                if (rep.lost_since(last_report)) notify("You lost points!");
            } catch (std::runtime_error &e) {
                log("E: score(): failed to generate report");
                log(string("E: ") + e.what());
                return;
            }
        }

        void stop_scoring() {
            if (!has("init")) {
                log("E: tried to stop scoring on an unintialized image");
                return;
            }
            if (status() != Status::Scoring) {
                log("E: not in scoring mode. Cannot stop scoring.");
                return;
            }
            if (status() == Status::Termination) {
                log("I: already stopped scoring");
                return;
            }

            set("stopped", "1");
            if (!has("final_report"))
                score();
        }

        int save() {
            log("I: saving config data");
            // write key-value pairs to CONFIG_FILE
            string plain = "";
            for (auto it = vals.begin(); it != vals.end(); it++) {
                plain += it->first + "=" + it->second + "\n";
            }
            // use the opposite value for exit code.
            bool ret = write_encrypted_file(CONFIG_FILE, plain, secret);
            if (!ret) log("E: failed to save config data");
            return !ret;
        }

    private:

        map<string,string> vals; // config
        Client client = Client(IMSC_URL); // connect to imsc server
        // hard-coded encryption key
        // extremely unsafe but whatever
        string secret = SECRET; 
        ofstream logf = ofstream(LOG_FILE, ofstream::out | ofstream::app);

        void notify(const string& msg) {


        }

        void add_time_info(ScoringReport& rep) const {
            rep.set_start_time(get("init_time"));
            rep.set_time_recorded(get_time_str());
            int mins = get_minutes_left();
            rep.set_time_left(std::to_string(mins/60) + " hr " +
                    std::to_string(mins%60) + " mins ");
        }

        bool reached_start_time() {
            if (!has("start_time")) {
                log("E: start_time not found");
                return false;
            }

            return time(nullptr) >= str_to_rawtime(get("start_time"));
        }

        bool reached_scoring_interval() {
            return time(nullptr) >
                str_to_rawtime(get("last_scored_time")) + SCORING_INTVL_MINS * 60;
        }

        static time_t str_to_rawtime(const string& dt) {
            tm t;
            strptime(dt.c_str(), "%Y-%m-%d %H:%M:%S", &t);
            return mktime(&t);
        }

        // get current time struct
        static tm get_time() {
            time_t rawtime;
            tm* ltime, ret;

            time(&rawtime);
            ltime = localtime(&rawtime);
            ret = *ltime;
            delete ltime;
            return ret;
        }

        // get formatted time
        static string get_time_str() {
            char buffer[80];
            tm lt = get_time();
            strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",&lt);
            return string(buffer);
        }

        bool read_data() {
            try {
                string keyvals = read_encrypted_file(CONFIG_FILE, secret);

                std::string key, val;
                std::istringstream iss(keyvals);

                // parse key-value pairs. stack overflow magic
                while(std::getline(std::getline(iss, key, '=') >> std::ws, val))
                    set(key,val);

                return true;
            } catch (FileError& e) {
                log(string("E: read_data(): ") + e.what());
                return false;
            }
        }

        Checklist make_checklist() {
            string chkls_str = read_encrypted_file(CHECKLIST_FILE, secret);
            std::istringstream iss(chkls_str);
            auto toml_chkls = toml::parse(iss);

            string title = toml::find<string>(toml_chkls, "title");
            string user = toml::find<string>(toml_chkls, "user");

            auto rules = toml::find<toml::array>(toml_chkls, "rules");

            Checklist chkls(title);
            for (const auto& rule : rules) {
                Rule r;
                r.name = toml::find<string>(rule, "name");
                r.pts = toml::find_or<int>(rule, "pts", 1);
                r.uniq = toml::find_or<string>(rule, "uniq", "");
                r.cmd = toml::find<string>(rule, "cmd");
                r.neg = toml::find_or<bool>(rule, "neg", false);
                r.preset = toml::find_or<string>(rule, "preset", "");
                r.args = toml::find_or<vector<string>>(rule, "args", {});
                chkls.add_rule(r);
            }
            return Checklist();
        }

        bool has(const string& key) const { return vals.count(key); }

        string get(const string& key) const { return vals.at(key); }

        void set(const string& key, const string& val) {
            vals.operator[](key);
            vals[key] = val;
        }
};

#endif
