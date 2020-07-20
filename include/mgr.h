#pragma once
#ifndef IMSC_SCORING_MANAGER_H
#define IMSC_SCORING_MANAGER_H

#include <string>

#include <curlpp/cURLpp.hpp>

#include "checklist.h"
#include "report.h"

enum class Status {
    Invalid, // server returned invalid status
    Wait, // wait for minimum scoring interval to elapse
    Score, // run score()
    Termination // end session
};

class ScoringManager {

    public:

        ScoringManager(const std::string& tok);
        static int session(const std::string& token);
        Status status();

    private:

        Checklist checklist;
        ScoringReport last_report;
        curlpp::Cleanup cleanup;
        std::string token = "";

        std::string start_time = "";
        std::string image_name = "";
        int duration = 0; // minutes
        int warn_mins = 15;

        static void notify(const std::string& msg);
        void score();
        int get_minutes_left() const;
};

#endif
