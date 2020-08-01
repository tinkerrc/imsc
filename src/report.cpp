#include "report.h"

#include <sstream>

#include "nlohmann/json.hpp"

using std::string;
using std::vector;
using json = nlohmann::json;

int ScoringReport::penal_pts() const {
    int pts = 0;
    for (const auto& itm : data["penalties"])
        pts += itm["pts"].get<int>();
    return pts;
}

int ScoringReport::vulns_pts() const {
    int pts = 0;
    for (const auto& itm : data["vulns"])
        pts += itm["pts"].get<int>();
    return pts;
}

bool ScoringReport::lost_since(const ScoringReport& last) const {
    return penal_pts() < last.penal_pts()
        || vulns_pts() < last.vulns_pts();
}

bool ScoringReport::gained_since(const ScoringReport& last) const {
    return penal_pts() >= last.penal_pts()
        && vulns_pts() > last.vulns_pts();
}

void ScoringReport::add_item(const json& itm) {
    if (itm["pts"].get<int>() > 0)
        data["vulns"].push_back(itm);
    else if (itm["pts"].get<int>() < 0)
        data["penalties"].push_back(itm);
    update_pts();
}
