#pragma once
#ifndef IMSC_TASK_H
#define IMSC_TASK_H

#include <string>
#include <iosfwd>
#include <variant>

// classes that allows passing back data to main()
// after argument processing is done
struct GenericTask {};
struct InitTask : GenericTask {
    std::string token;
    InitTask(const std::string &tok) : token(tok) {}
};
struct TimeTask : GenericTask {};
struct StopScoringTask : GenericTask {};
struct ScoreTask : GenericTask {};

// NOTE: remember to update the constants
typedef std::variant<InitTask,
        TimeTask,
        StopScoringTask,
        ScoreTask> Task;

// for use with Task::index()
const std::size_t TASK_INIT = 0;
const std::size_t TASK_TIME = 1;
const std::size_t TASK_STOP_SCORING = 2;
const std::size_t TASK_SCORE = 3;

// parse arguments
Task get_task(int argc, char *argv[]);

#endif
