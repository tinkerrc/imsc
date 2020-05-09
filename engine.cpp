#define DEBUG 1

#include <string>
#include <unistd.h>

#include "task.h"
#include "mgr.h"

using std::string;
using std::vector;
using std::cout;

/*
 *  1.     user launches image
 *  2. [ ] cron runs imsc
 *  ---
 *  3. [ ] imsc records image launch time
 *  4. [ ] user runs imsc --token TOKEN
 *  5. [ ] mgr decrypts and start the timer
 *  6. [ ] contact server, send token, store starting time
 *         send initial score() report to server
 *  7. [ ] cron job : * / 1 * * * * * imsc --score 
 *  8. [ ] score() tells the server the score
 *  8. [ ] score() call stop_scoring() once grace time reached
 *         and contact server with final report
 *  9. [ ] generate final report on desktop and output an encrypted
 *         version as well.
 *
 */

ScoringManager mgr;

void unexpected() {
    mgr.save();
}

int main(int argc, char *argv[]) {
    std::set_unexpected(unexpected);
    try {
        Task t = get_task(argc, argv);
        switch (t.index()) {
            case TASK_INIT:
                mgr.init_img(std::get<InitTask>(t).token);
                break;
            case TASK_STOP_SCORING:
                mgr.stop_scoring();
                break;
            case TASK_SCORE:
#ifndef DEBUG
                if (getuid() != 0) {
                    cout << "Must score using root account.\n";
                    exit(1);
                }
                mgr.score();
#endif
            case TASK_TIME:
            default:
                cout << "WHS CSC Image Scoring Engine for Linux\n";
                if (mgr.status() == ScoringManager::Scoring) {
                    int mins_left = mgr.get_minutes_left();
                    if (mins_left == -1) {
                        std::cerr << "The image has not yet been initialized.\n";
                        exit(1);
                    }
                    int hr = mins_left/60;
                    int mins = mins_left%60;
                    cout << "Time remaining: " << hr << " hours " << mins << " minutes\n";
                }
                else {
                    std::cerr << "Please initialize the image using your token.\n";
                }
        }

    } catch (std::exception& e) {
        mgr.log(string("E: main(): ") + e.what());
    }
    return mgr.save();
}
