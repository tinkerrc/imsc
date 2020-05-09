#include "checklist.h"

#include <numeric>
#include <sys/wait.h>

using std::string;
using std::vector;

bool Rule::satisfied() const {
    int ret = system(cmd.c_str());
    bool success = WEXITSTATUS(ret) == 0x10;
    return neg? !success : success;
}

void Checklist::add_rule(const Rule& r)  {
    rules.push_back(r);
}

ScoringReport Checklist::check() const {
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
