#pragma once
#ifndef IMSC_CHECKLIST_H
#define IMSC_CHECKLIST_H

#include <string>
#include <vector>

#include "report.h"

struct Rule {
    std::string name;
    // distinguish b/w others w/ same name
    std::string uniq; // might be useless
    int pts = 0;
    std::string cmd;
    bool neg = false; // negates cmd exit code
    // reserved
    std::string preset;
    std::vector<std::string> args;

    bool satisfied() const;
};

class Checklist {
    private:
        std::string title;
        std::vector<Rule> rules;
        
    public:
        Checklist() = default;
        Checklist(const std::string& title)
            : title(title) {}

        void add_rule(const Rule& r);

        // iterate the checklist and generate a report
        ScoringReport check() const;
};

#endif
