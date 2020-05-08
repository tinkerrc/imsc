#ifndef H_ARG_PARSER
#define H_ARG_PARSER

#include <boost/program_options.hpp>
#include <iostream>
#include <variant>

namespace po = boost::program_options;

const std::size_t TASK_INIT = 0;
const std::size_t TASK_GEN_REPORT = 1;
const std::size_t TASK_TIME = 2;
const std::size_t TASK_STOP_SCORING = 3;
const std::size_t TASK_SCORE = 4;

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
typedef std::variant<InitTask,
        GenerateReportTask,
        TimeTask,
        StopScoringTask,
        ScoreTask> Task;

Task get_task(int argc, char *argv[]) {
    try { 
        po::options_description opts("Allowed options");
        opts.add_options()
            ("help,h", "show help message")
            ("token,t", po::value<std::string>(), "provide token to start scoring")
            ("stop", "stop scoring and generate result hash");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, opts), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << opts << "\n";
            exit(0);
        }

        if (vm.count("stop"))
            return StopScoringTask();
        else if (vm.count("token"))
            return InitTask(vm["token"].as<std::string>());
        else 
            return TimeTask();
    }
    catch(...) {
        std::cerr << "E: failed to process options\nContinuing with default task.\n";
        return TimeTask();
    }
}

#endif // H_ARG_PARSER
