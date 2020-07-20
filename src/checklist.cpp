#include "checklist.h"
#include "utils.h"

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
    int num_vulns = 0;

    Log() << "Running all checkers...";
    for (const auto& rule : rules)
        if (rule.satisfied())
            rep.add_item({{"id", rule.id},
                          {"name", rule.name},
                          {"pts", rule.pts}});

    return rep;
}
