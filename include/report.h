#pragma once
#ifndef H_REPORT
#define H_REPORT

#include <string>
#include <vector>

struct ScoredItem {
    std::string name;
    std::string uniq;
    int pts = 0;
};

class ScoringReport {
    private:
        std::string start_time;
        std::string time_recorded;
        std::string time_left;

        std::vector<ScoredItem> vulns; // pos pts
        std::vector<ScoredItem> penalties; // neg pts

        std::string title; // title of practice image
        int max_pts = 0; // max points possible
        int total_vulns = 0; // total # of vulns

    public:
        ScoringReport() = default;
        ScoringReport(const std::string& data);

        // returns human-readable data
        std::string to_string(bool use_uniq = false) const;

        void set_title(const std::string& s) { title = s; }
        void set_max_pts(int p) { max_pts = p; }
        void set_total_vulns(int n) { total_vulns = n; }
        void set_start_time(const std::string& s) { start_time = s; }
        void set_time_recorded(const std::string& s) { time_recorded = s; }
        void set_time_left(const std::string& s) { time_left = s; }

        std::string summary() const;

        // returns parsable data
        std::string data() const;

        int pts() const { return penal_pts() + vulns_pts(); }

        int penal_pts() const;

        int vulns_pts() const;

        bool lost_since(const ScoringReport& last) const;

        bool gained_since(const ScoringReport& last) const;

        void add_item(const ScoredItem& itm);
};

#endif
