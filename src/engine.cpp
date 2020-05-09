#define DEBUG 1

#include <string>

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

int main(int argc, char *argv[]) {
    int ret;
    try {
        ScoringManager mgr;
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
#endif
                mgr.score();
            case TASK_TIME:
            default:
                cout << "WHS CSC Image Scoring Engine for Linux\n";
                if (mgr.status() == ScoringManager::Scoring) {
                    int mins_left = mgr.get_minutes_left();
                    if (mins_left == -1) {
                        std::cerr << "The image has not yet been initialized.\n";
                    }
                    else {
                        int hr = mins_left/60;
                        int mins = mins_left%60;
                        cout << "Time remaining: " << hr << " hours " << mins << " minutes\n";
                    }
                }
                else {
                    std::cerr << "Please initialize the image using your token.\n";
                }
        }
        ret = mgr.save();
    } catch (std::exception& e) {
        std::cerr << "E: " << e.what() << "\n";
        std::cerr << "E: Terminating.\n";
    }
    return ret;
}
