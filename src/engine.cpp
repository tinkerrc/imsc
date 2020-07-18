#include <vector>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/ptrace.h>
#include <thread>

#include "mgr.h"
#include "utils.h"
#include "config.h"

using std::string;
using std::vector;
using std::cout;

int main(int argc, char *argv[]) {
    int offset = 0;
    offset = (!ptrace(PTRACE_TRACEME, 0, 1, 0))?2:0;
    offset *= ptrace(PTRACE_TRACEME, 0, 1, 0) == -1?3:1;
    if (offset != 6) {
        Err() << "Please...";
        exit(1);
    }
    try {
        cout << "---==== imsc " << IMSC_VERSION << " ====---\n";

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
        for(;;) {
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
