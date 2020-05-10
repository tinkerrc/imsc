#define DEBUG 1

#include <vector>
#include <iostream>
#include <string>

#include "task.h"
#include "mgr.h"
#include "utils.h"
#include "config.h"

using std::string;
using std::vector;
using std::cout;

/*
 *  1.     user launches image
 *  2. [-] cron runs imsc
 *  ---
 *  3. [x] imsc records image launch time
 *  4. [ ] user runs imsc --token TOKEN
 *  5. [x] mgr decrypts and start the timer
 *  6. [ ] contact server, send token, store starting time
 *         send initial score() report to server
 *  7. [-] cron job : * / 1 * * * * * imsc --score 
 *  8. [ ] score() tells the server the score
 *  8. [x] score() calls stop_scoring() once grace period is reached
 *         and contacts server with final report
 *  9. [-] generate final report on desktop and output an encrypted
 *         version as well.
 *
 */

int main(int argc, char *argv[]) {
    int ret;
    try {
        Log() << "---==== imsc " << imsc_VERSION << " ====---";
        Task t = get_task(argc, argv);
        ScoringManager mgr;
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
                        Log() << "E: The image has not yet been initialized.";
                    }
                    else {
                        int hr = mins_left/60;
                        int mins = mins_left%60;
                        cout << "Time remaining: " << hr << " hours " << mins << " minutes\n";
                    }
                }
                else {
                    Log() << "E: Please initialize the image using your token.\n";
                }
        }
        ret = mgr.save();
    } catch (std::exception& e) {
        Log() << "E: " << e.what();
        Log() << "E: Terminating.";
    }
    return ret;
}
