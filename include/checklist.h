#pragma once
#ifndef IMSC_CHECKLIST_H
#define IMSC_CHECKLIST_H

#include <string>
#include <vector>

#include "report.h"

struct Rule {
    int id = 0;
    std::string name;
    int pts = 0;
    std::string cmd;
    int code = 0;

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
