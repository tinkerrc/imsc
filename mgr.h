#ifndef H_SCORING_MANAGER
#define H_SCORING_MANAGER

#include <string>
#include <map>
#include <ctime>
#include <fstream>

#include <curlpp/Easy.hpp>
#include <curlpp/cURLpp.hpp>
#include <toml/parser.hpp>

#include "file.h"
#include "checklist.h"
#include "config.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::map;

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
            // obtain secret from server (prospective)
            if (!get_secret())
                throw std::runtime_error("ScoringManager(): Failed to obtain key.");
            // read run-specific data
            if (!read_data())
                throw std::runtime_error("ScoringManager(): Failed to read data. Contact proctor.");
            // record image launch time
            if (!has("launch_time"))
                set("launch_time", get_time_str());
        }

        ~ScoringManager() {
            save();
        }

        void log(const string& str) {

        }

        /* initialize the image using token
         * 0 = success
         * 1 = already initialized
         * 2 = invalid token
         */
        int init_img(const string &token) {
            // TODO: check if image is initialized
            // by checking DATA_FILE's init=1
            // TODO: write init=1
            // TODO: record initialization time
            // TODO: store encrypted config in variable
            // TODO: decrypt cipher using AES & SECRET
            // TODO: parse using toml::parse
            // TODO: if start time is not reached, exit
            // TODO: execute init script in config
            // TODO: setup forensics questions
            // TODO: generate readme file on user desktop
            // TODO: store generate_checklist(plain_text_conf)
            return 2;
        }

        int get_minutes_left() const {
            // TODO: subtract true starting time from current time
            if (!has("init")) {
                throw std::runtime_error("get_minutes_left(): the image has not been initialized yet.");
            }
            return -1;
        }

        Status status() {
            if (has("init") && has("config")) {
                if (get_minutes_left() > 0)
                    return Status::Scoring;
                else if (std::atoi(get("grace_time").c_str()) >= -get_minutes_left())
                    return has("stopped")? Status::Grace : Status::FinalReport;
                else
                    return Status::Termination;
            }
            return Status::Timed; // timed even if not initialized
        }

        void score() {
            if (!vals.count("init")) {
                throw std::runtime_error("score(): Image has not been initialized yet.");
            }
            // TODO: check if time period had changed, act accordingly
            // TODO: check in entries.last_scored whether 1m had elapsed
            // TODO: if entered grace period / termination, do not score then quit
            Report rep = read_config().check();
            // TODO: figure out user desktop
            write_file("", rep.to_string());
            // TODO: if appropriate, run stop scoring
            // TODO: return report
            // TODO: record pts, if pts > last time, output good sound, if pts < last time, bad sound
            // TODO: if has penalty, output bad sound
            // TODO: compute current report cipher and store it in DATA_FILE
        }

        void stop_scoring() {
            // TODO: check if initialized, else exit(1)
            // TODO: if stopped is set, return
            //       otherwise set stopped = true
            // TODO: call score() for one last time and send the results to server
            //       if vals.final_report has not been set
        }

        int save() {
            // TODO: write key value pairs to DATA_FILE
            return 0;
        }

    private:

        curlpp::Cleanup cleanup;
        map<string,string> vals;
        Checklist chkls;
        string secret;
        ofstream logf;

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
            strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",&lt);
            return string(buffer);
        }

        bool get_secret() {
            // TODO: obtain secret from server instead
            secret = SECRET;
            return true;
        }

        bool read_data() {
            try {
                string keyvals = read_encrypted_file(DATA_FILE, secret);

                std::string key, val;
                std::istringstream iss(keyvals);

                while(std::getline(std::getline(iss, key, '=') >> std::ws, val))
                    set(key,val);

                return true;
            } catch (std::runtime_error& e) {
                return false;
            }
            // TODO: record current time if called first-time
        }

        Checklist read_config() {
            try {
                string config = read_encrypted_file(CONFIG_FILE, secret);
                auto toml = "";
                // MEGA TODO: parse config
                return Checklist();
            } catch (std::runtime_error& e) {
                log("E: read_config(): " + string(e.what()));
                throw e;
            }
        }

        bool has(const string& key) const { return vals.count(key); }

        string get(const string& key) const { return vals.at(key); }

        void set(const string& key, const string& val) {
            vals[key] = val;
        }
};

#endif
