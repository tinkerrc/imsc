#ifndef H_TASK
#define H_TASK

#include <iostream>
#include <variant>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

// classes that allows passing back data to main()
// after argument processing is done
struct GenericTask {};
struct InitTask : GenericTask {
    std::string token;
    InitTask(const std::string &tok) : token(tok) {}
};
struct GenerateReportTask : GenericTask {};
struct TimeTask : GenericTask {};
struct StopScoringTask : GenericTask {};
struct ScoreTask : GenericTask {};

// NOTE: remember to update the constants
typedef std::variant<InitTask,
        GenerateReportTask,
        TimeTask,
        StopScoringTask,
        ScoreTask> Task;

// for use with Task::index()
const std::size_t TASK_INIT = 0;
const std::size_t TASK_GEN_REPORT = 1;
const std::size_t TASK_TIME = 2;
const std::size_t TASK_STOP_SCORING = 3;
const std::size_t TASK_SCORE = 4;

// basically parse arguments
Task get_task(int argc, char *argv[]) {
    try { 
        // setup Boost program options
        po::options_description opts("Allowed options");
        opts.add_options()
            ("help,h", "show help message")
            ("token,t", po::value<std::string>(), "initialize the image using a token")
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
        else 
            return TimeTask();
    }
    catch(...) {
        std::cerr << "E: failed to process options\nE: Continuing with default task.\n";
        return TimeTask();
    }
}

#endif
