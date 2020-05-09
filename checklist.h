#ifndef H_CHECKLIST
#define H_CHECKLIST

#include <string>
#include <vector>
#include <stdexcept>
#include <sys/wait.h>

#include <boost/algorithm/string/classification.hpp> // boost::for is_any_of
#include <boost/algorithm/string/split.hpp> // boost::split

#include "report.h"

using std::string;
using std::vector;

struct Rule {
    string name = "";
    // distinguish b/w others w/ same name
    string uniq = "";
    int pts = 0;
    string cmd = "";
    bool neg = false; // negates cmd exit code
    // reserved
    string preset = "";
    vector<string> args;

    bool check() const {
        int ret = system(cmd.c_str());
        bool success = WEXITSTATUS(ret) == 0x10;
        return neg? !success : success;
    }
};

class Checklist {
    private:
        vector<Rule> rules;
        
    public:
        Checklist() = default;

        void add_rule(const Rule& r) {
            rules.push_back(r);
        }

        // iterate the checklist and generate a report
        Report check() { 
            // TODO: implement this
            return Report(); 
        }
};

#endif
