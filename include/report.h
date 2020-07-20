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

        ScoringReport() {
            data["pts"] = 0;
            data["vulns"] = nlohmann::json::parse("[]");
            data["penalties"] = nlohmann::json::parse("[]");
        }

        ScoringReport(const nlohmann::json& d)
            : data(d) {}

        ScoringReport(nlohmann::json&& d)
            : data(d) {}

        nlohmann::json to_json() { return (nlohmann::json)(*this); }
        operator nlohmann::json() { return data; }

        int pts() const { return penal_pts() + vulns_pts(); }
        int penal_pts() const;
        int vulns_pts() const;
        void update_pts() {
            int p = pts();
            data["pts"] = p;
        }

        bool lost_since(const ScoringReport& last) const;
        bool gained_since(const ScoringReport& last) const;
        void add_item(const nlohmann::json& itm);
};

#endif
