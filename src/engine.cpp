#define DEBUG 1

#include <vector>
#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>

#include "mgr.h"
#include "utils.h"
#include "config.h"

using std::string;
using std::vector;
using std::cout;

/*
 * setup the image first
 * put the scoring report link on desktop
 * ...
 * [ ] GET: checklist (JSON), config using token 
 * [ ] generate scoring report in JSON
 * [ ] POST: report
 * [ ] sleep
 * [ ] repeat
 */

int main(int argc, char *argv[]) {
    try {
        cout << "---==== imsc " << IMSC_VERSION << " ====---";
        cout << "GitHub: oakrc/imsc";

        if (argc != 2)
            throw std::runtime_error("Please supply a token");

        if (string("-h") == argv[1] || string("--help") == argv[1]) {
            cout << "Usage: " << argv[0] << " TOKEN\n";
            return 0;
        }
        
        if (geteuid() != 0) {
            throw std::runtime_error("imsc needs root privileges to score");
        }
        ScoringManager mgr(argv[1]);
        Status s;
        while (1) {
            s = mgr.status();
            if (s == Status::Termination || s == Status::Invalid) {
                Log() << "Terminating.";
                return 0;
            }
            Log() << "Scoring...";
            mgr.score();
            Log() << "Sleeping...";
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    } catch (const std::exception& e) {
        Err() << e.what();
        Err() << "Terminating.";
    }
    return 0;
}
