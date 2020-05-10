#pragma once
#ifndef IMSC_SCORING_MANAGER_H
#define IMSC_SCORING_MANAGER_H

#include <string>
#include <map>

#include "client.h"

class Checklist;
class ScoringReport;

class UnallowedActionError : public std::runtime_error {
    public:
        UnallowedActionError(const std::string& s)
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

        ScoringManager();

        ~ScoringManager();

        /* initialize the image using token
         * 0 = success
         * 1 = already initialized
         * 2 = invalid token
         * 3 = not available yet
         * 4 = error
         */
        int init_img(const std::string &token);

        /* returns minutes left til scoring stops
         * negative numbers mean minutes since scoring stopped 
         */
        int get_minutes_left() const;

        Status status() const;

        void score();

        void stop_scoring();

        int save() const;

    private:

        std::map<std::string,std::string> vals; // config
        Client client; // connect to imsc server
        std::string secret; // hardcoded

        void notify(const std::string& msg) const;

        void add_time_info(ScoringReport& rep) const;

        bool reached_start_time() const;

        bool reached_scoring_interval() const;

        bool read_data();

        Checklist make_checklist() const;

        bool has(const std::string& key) const { return vals.count(key); }

        std::string get(const std::string& key) const { return vals.at(key); }

        void set(const std::string& key, const std::string& val) {
            vals[key] = val;
        }
};

#endif
