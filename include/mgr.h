#pragma once
#ifndef IMSC_SCORING_MANAGER_H
#define IMSC_SCORING_MANAGER_H

#include <string>

#include <curlpp/cURLpp.hpp>
#include "checklist.h"
#include "report.h"

enum class Status {
    Invalid,
    Wait,
    Score,
    Termination
};

// the scoring engine
// Checklist is the one that actually scores
class ScoringManager {

    public:

        ScoringManager(const std::string& tok);

        void score();

        Status status();

    private:

        Checklist checklist;
        ScoringReport last_report;
        curlpp::Cleanup cleanup;
        std::string token;

        std::string start_time;
        std::string user;
        std::string image_name;
        int duration = 0; // minutes
        int warn_mins = 15;

        void notify(const std::string& msg) const;

        int get_minutes_left() const;

        void make_checklist(const std::string& chkls_str);

        void send_report(const ScoringReport& report) const;
};

#endif
