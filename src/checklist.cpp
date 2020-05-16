#include "checklist.h"

#include <numeric>
#include <sys/wait.h>

using std::string;
using std::vector;

bool Rule::satisfied() const {
    int ret = system(cmd.c_str());
#ifdef __linux__
    return WEXITSTATUS(ret) == code;
#elif defined(_WIN32)
#   error "unimplemented"
#endif
}

void Checklist::add_rule(const Rule& r) {
    rules.push_back(r);
}

ScoringReport Checklist::check() const {
    ScoringReport rep;
    rep.set_title(title);
    int num_vulns = 0;
    rep.set_max_pts(std::accumulate(
                    rules.begin(), rules.end(), 0,
                    [&] (int i, Rule r) {
                    return r.pts > 0? (num_vulns++, i + r.pts) : i;
                }));
    rep.set_total_vulns(num_vulns);

    for (const auto& rule : rules)
        if (rule.satisfied())
            rep.add_item({rule.name, rule.uniq, rule.pts});

    return rep;
}
