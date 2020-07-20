#pragma once
#ifndef IMSC_CHECKLIST_H
#define IMSC_CHECKLIST_H

#include <string>
#include <vector>

#include "report.h"

struct Rule {
    std::string id = "";
    std::string name = "Rule";
    int pts = 0;
    std::string cmd = "false";
    int code = 0;

    bool satisfied() const;
};

class Checklist {

    private:

        std::vector<Rule> rules;

    public:

        void add_rule(const Rule& r);

        // iterate the checklist and generate a report
        ScoringReport check() const;
};

#endif
