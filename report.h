#ifndef H_REPORT
#define H_REPORT

#include <string>
#include <vector>
using std::string;
using std::vector;
class Report {
    private:
        string time;
        vector<string> vulns;

    public:
        Report() {}
        Report(const string& data) {}

        // returns human-readable data
        string to_string() {
            return ""; 
        }

        // returns parsable data
        string data() {
            // TODO: return a string that can be used to create Report later
            return "";
        }

        // TODO: compare two reports
        bool operator<(const Report& rhs) {
            return false;
        }
};

#endif
