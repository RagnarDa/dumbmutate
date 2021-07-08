//
// Created by chris on 2021-07-07.
//

#include "CommandRunner.h"

#ifdef WIN32
// Borrowed from here: https://github.com/cernoch/tuxliketimeout/blob/master/tuxliketimeout.cpp
#include <limits>
#include <string>
#include <iostream>
#include <windows.h>
#include <IntSafe.h>


/**
 * Calls a clean-up method in the destructor.
 *
 * Allocate this object to ensure that any subsequent
 * `return` calls the `CloseHandle` on the given handle.
 */
struct HandleGuard {

    HANDLE m_handle;

    HandleGuard(HANDLE handle)
            : m_handle(handle) {}

    ~HandleGuard() {
        CloseHandle(m_handle);
    }
};

CommandRunner::CommandResult CommandRunner::RunCommand(const char *command, int timeout) {
    if (timeout <= 0)
    {
        return system(command) == 0 ? CommandResult::CommandCompleted : CommandResult::CommandErrorCodeNonZero;
    }
    wchar_t wcommand[1024];
    mbstowcs(wcommand, command, strlen(command)+1);
    LPWSTR command_line_buf = const_cast<wchar_t*>(wcommand);

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process.
    if (!CreateProcessW(
            NULL,           // No module name (use command line)
            command_line_buf, // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            0,              // No creation flags
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory
            &si,            // Pointer to STARTUPINFO structure
            &pi )           // Pointer to PROCESS_INFORMATION structure
            ) {
        switch (GetLastError()) {

            case ERROR_FILE_NOT_FOUND:
                return CommandResult::CommandNotFound;

            default:
                return CommandResult::ErrorExecuting;
        }
    }

    // Close all handles on any path
    HandleGuard hProcessGuard(pi.hProcess);
    HandleGuard hThreadGuard(pi.hThread);

    // Wait until child process exits.
    switch (WaitForSingleObject(pi.hProcess, timeout)) {

        case WAIT_FAILED:
            std::wcerr << L"WaitForSingleObject failed. (ERROR ";
            std::wcerr << GetLastError() << L")" << std::endl;
            return CommandResult::InternalError;

        case WAIT_TIMEOUT:
            if (!TerminateProcess(pi.hProcess, 0)) {
                std::wcerr << L"TerminateProcess failed. (ERROR ";
                std::wcerr << GetLastError() << L")" << std::endl;
                return CommandResult::InternalError;
            }
            if (!WaitForSingleObject(pi.hThread, 0)) {
                std::wcerr << L"WaitForSingleObject failed. (ERROR ";
                std::wcerr << GetLastError() << L")" << std::endl;
                return CommandResult::InternalError;
            }
            return CommandResult::CommandTimedout;

        case WAIT_OBJECT_0:
            DWORD error_code;
            if (GetExitCodeProcess(pi.hProcess, &error_code)) {
                return error_code == 0 ?
                    CommandResult::CommandCompleted
                    :CommandResult::CommandErrorCodeNonZero;
            } else {
                std::wcerr << L"GetExitCodeProcess failed. (ERROR ";
                std::wcerr << GetLastError() << L")" << std::endl;
                return CommandResult::InternalError;
            }

        default:
            std::wcerr << L"WaitForSingleObject returned an unexpected ";
            std::wcerr << L" value (" << GetLastError() << L")." << std::endl;
            return CommandResult::InternalError;
    }
}
#endif


