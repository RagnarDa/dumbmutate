//
// Created by chris on 2021-07-07.
//

#ifndef DUMBMUTATE_COMMANDRUNNER_H
#define DUMBMUTATE_COMMANDRUNNER_H


#include <string>

class CommandRunner {
public:

    enum class CommandResult
    {
        CommandResultZero = 0,
        CommandTimedout = 1,
        InternalError = 2,
        ErrorExecuting = 3,
        CommandNotFound = 4,
        CommandResultCodeNonZero = 5, // If the process ran but returned a errorcode, i e mutation killed
    };
    static CommandRunner::CommandResult RunCommand(std::string command, int timeoutms, bool suppressoutput);
};


#endif //DUMBMUTATE_COMMANDRUNNER_H
