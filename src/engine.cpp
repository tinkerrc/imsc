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
    if (geteuid() != 0) {
        Err() << "imsc needs root privileges to score";
        return 1;
    }

    cout << "---==== imsc " << IMSC_VERSION << " ====---\n";

    if (argc != 2 || string("-h") == argv[1] || string("--help") == argv[1]) {
        cout << "Usage: " << argv[0] << " TOKEN\n";
        return 1;
    }
        
    return ScoringManager::session(argv[1]);
}
