//
// Created by chris on 2021-07-07.
//

#ifndef DUMBMUTATE_COMMANDRUNNER_H
#define DUMBMUTATE_COMMANDRUNNER_H


class CommandRunner {
    enum class CommandResult
    {
        CommandResultZero = 0,
        CommandTimedout = 1,
        InternalError = 2,
        ErrorExecuting = 3,
        CommandNotFound = 4,
        CommandResultCodeNonZero = 5, // If the process ran but returned a errorcode
    };
public:
    static CommandResult RunCommand(const char * command, int timeoutms = 0);
};


#endif //DUMBMUTATE_COMMANDRUNNER_H
