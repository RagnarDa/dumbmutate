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
        return system(command) == 0 ? CommandResult::CommandResultZero : CommandResult::CommandResultCodeNonZero;
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
                    CommandResult::CommandResultZero
                    :CommandResult::CommandResultCodeNonZero;
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
#else

// POSIX/LINUX/MAC
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <atomic>
#include <csignal>
#include <thread>

using std::cout; using std::endl;
using std::string;

constexpr std::atomic<int> handler_exit_code(103);

std::atomic<int> child_pid;


void SigQuitHandler(int signal_number) {
    kill(child_pid, SIGTERM);
    while ((child_pid = wait(nullptr)) > 0);
    _exit(handler_exit_code);
}


pid_t SpawnChild(std::string command) {
    pid_t ch_pid = fork();
    if (ch_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (ch_pid > 0) {
        //cout << "spawn child with pid - " << ch_pid << endl;
        return ch_pid;
    } else {
        // Assign the process to a process group that will allow all processes be killed.
        setpgid(getpid(), getpid());

        int returncode = system(command.c_str());
        //std::cout << "Return code: " << returncode << std::endl;
        exit(returncode);
    }
}

CommandRunner::CommandResult CommandRunner::RunCommand(const char *command, int timeoutms) {
    // In case we are signaled to quit while this is running
    signal(SIGQUIT, SigQuitHandler);

    if (timeoutms <= 0) {
        // Regular old system() when no timeout
        return system(command) == 0
            ? CommandResult::CommandResultZero
            : CommandResult::CommandResultCodeNonZero;
    }

    // Spawn child
    child_pid = SpawnChild(command);
    int status;

    // A spin wait
    while ((waitpid(child_pid, &status, WNOHANG)) == 0 && timeoutms > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));;
        timeoutms--;
    }
    if (timeoutms <= 0) {
        std::cout << "Process timed out, killing..." << std::endl;
        // Not sure why but you need to add the minus for process group
        kill(-child_pid, SIGKILL);
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status) == 0 ?
               CommandResult::CommandResultZero
                               :CommandResult::CommandResultCodeNonZero;
    } else if (WIFSIGNALED(status) && timeoutms <= 0) {
        return CommandResult::CommandTimedout;
    } else if (WIFSTOPPED(status) && timeoutms <= 0) {
        return CommandResult::CommandTimedout;
    } else {
        throw std::exception();
    }
}
#endif


