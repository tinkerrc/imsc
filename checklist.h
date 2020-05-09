#ifndef H_CHECKLIST
#define H_CHECKLIST

#include <string>
#include <vector>
#include <numeric>
#include <sys/wait.h>

#include "report.h"

using std::string;
using std::vector;

struct Rule {
    string name = "";
    // distinguish b/w others w/ same name
    string uniq = ""; // might be useless
    int pts = 0;
    string cmd = "";
    bool neg = false; // negates cmd exit code
    // reserved
    string preset = "";
    vector<string> args;

    bool satisfied() const {
        int ret = system(cmd.c_str());
        bool success = WEXITSTATUS(ret) == 0x10;
        return neg? !success : success;
    }
};

class Checklist {
    private:
        string title = "";
        vector<Rule> rules;
        
    public:
        Checklist() = default;
        Checklist(const string& title)
            : title(title) {}

        void add_rule(const Rule& r) {
            rules.push_back(r);
        }

        // iterate the checklist and generate a report
        ScoringReport check() const {
            ScoringReport rep;
            rep.set_title(title);
            int n_vulns = 0;
            rep.set_max_pts(std::accumulate(
                        rules.begin(), rules.end(), 0,
                        [&] (int i, Rule r) {
                            return r.pts > 0? (n_vulns++, i + r.pts) : i;
                        }));
            rep.set_total_vulns(n_vulns);

            for (const auto& rule : rules) {
                if (rule.satisfied())
                    rep.add_item({rule.name, rule.uniq, rule.pts});
            }
            return rep;
        }
};

#endif
