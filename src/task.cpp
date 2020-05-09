#include "task.h"
#include "utils.h"

#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

Task get_task(int argc, char *argv[]) {
    try { 
        // setup Boost program options
        po::options_description opts("Allowed options");
        opts.add_options()
            ("help,h", "show help message")
            ("time,t", "show time left (default)")
            ("token,T", po::value<std::string>(), "initialize the image using a token")
            ("score,s", "score the image and send results to server")
            ("stop,X", "stop scoring and generate final report");

        // parse
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, opts), vm);
        po::notify(vm);

        // interpret
        if (vm.count("help")) {
            std::cout << opts << "\n";
            exit(0);
        }

        if (vm.count("stop"))
            return StopScoringTask();
        else if (vm.count("token"))
            return InitTask(vm["token"].as<std::string>());
        else if (vm.count("score"))
            return ScoreTask();
    } catch(po::error& e) {
        Log() << "E: options invalid";
    } catch(...) {
        Log() << "E: could not process options";
    }
    Log() << "I: continuing with default task";
    return TimeTask();
}
