#pragma once
#ifndef IMSC_REPORT_H
#define IMSC_REPORT_H

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

// a wrapper class for json
class ScoringReport {
    private:
        nlohmann::json data;

    public:
        ScoringReport() = default;

        ScoringReport(const nlohmann::json& d)
            : data(d) {}

        ScoringReport(nlohmann::json&& d)
            : data(d) {}

        // returns human-readable data
        std::string to_string() const;

        void set_title(const std::string& s) { data["title"] = s; }
        void set_max_pts(int p) { data["max_pts"] = p; }
        void set_total_vulns(int n) { data["total_vulns"] = n; }
        void set_start_time(const std::string& s) { data["start_time"] = s; }
        void set_time_recorded(const std::string& s) { data["time_recorded"] = s; }
        void set_time_left(const std::string& s) { data["time_left"] = s; }

        nlohmann::json to_json() { return (json)(*this); }
        operator nlohmann::json() {
            data["pts"] = pts();
            return data;
        }

        int pts() const { return penal_pts() + vulns_pts(); }

        int penal_pts() const;

        int vulns_pts() const;

        bool lost_since(const ScoringReport& last) const;

        bool gained_since(const ScoringReport& last) const;

        void add_item(const nlohmann::json& itm);
};

#endif
