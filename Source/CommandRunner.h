//
// Created by chris on 2021-07-07.
//

#ifndef DUMBMUTATE_COMMANDRUNNER_H
#define DUMBMUTATE_COMMANDRUNNER_H


class CommandRunner {
    enum class CommandResult
    {
        CommandCompleted = 0,
        CommandTimedout = 1,
        InternalError = 2,
        ErrorExecuting = 3,
        CommandNotFound = 4,
        CommandErrorCodeNonZero = 5, // If the process ran but returned a errorcode
    };
public:
    static CommandResult RunCommand(const char * command, int timeout = 0);
};


#endif //DUMBMUTATE_COMMANDRUNNER_H
