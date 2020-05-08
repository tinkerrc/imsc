#ifndef H_CHECKLIST
#define H_CHECKLIST

#include <string>
#include <vector>
#include <stdexcept>
#include "report.h"
#include <toml/parser.hpp>

using std::string;
using std::vector;

struct Rule {
    string name = "";
    int pts = 0;

    string cmd = "";
    // if neg == true and 
    // cmd exits with non-zero value,
    // pts will be applied
    bool neg = false;     

    // reserved
    string preset = "";
    vector<string> args;
};

class Checklist {
    private:
        
        
    public:
        Checklist(const std::string& config) {
            // assume the config is syntatically correct
            if (!configure(config))
                throw std::runtime_error("Failed to configure checklist");
        }
        Checklist(){}

        bool configure(const std::string &config) {

            return true;
        }

        // iterate the checklist and generate a report
        Report check() { 
            return Report(); 
        }
};

#endif
