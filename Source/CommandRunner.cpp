//
// Created by chris on 2021-07-07.
//

#include "CommandRunner.h"

#ifdef WIN32
// Borrowed from here: https://github.com/cernoch/tuxliketimeout/blob/master/tuxliketimeout.cpp
// and here: https://docs.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output
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
#define BUFSIZE 4096

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

HANDLE g_hInputFile = NULL;
// TODO: Should this method be called? Maybe clearing the pipe?
void ReadFromPipe(void)

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT.
// Stop when there is no more data.
{
    DWORD dwRead, dwWritten;
    CHAR chBuf[BUFSIZE];
    BOOL bSuccess = FALSE;
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    for (;;)
    {
        bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if( ! bSuccess || dwRead == 0 ) break;

        bSuccess = WriteFile(hParentStdOut, chBuf,
                             dwRead, &dwWritten, NULL);
        if (! bSuccess ) break;
    }
}

CommandRunner::CommandResult CommandRunner::RunCommand(std::string command, int timeout, bool suppressoutput) {


    if (timeout <= 0)
    {
        // Pipe output to /dev/null
        if (suppressoutput)
        {
            command.append(" > nul 0> nul");
        }
        return system(command.c_str()) == 0 ? CommandResult::CommandResultZero : CommandResult::CommandResultCodeNonZero;
    }

    // We need to pipe output to be able to suppress it
    SECURITY_ATTRIBUTES saAttr;


// Set the bInheritHandle flag so pipe handles are inherited.

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

// Create a pipe for the child process's STDOUT.

    if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) )
        exit(EXIT_FAILURE);

// Ensure the read handle to the pipe for STDOUT is not inherited.

    if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
        exit(EXIT_FAILURE);

    wchar_t wcommand[1024];
    size_t size = strlen(command.c_str()) + 1;
    size_t outSize;
    mbstowcs_s(&outSize, wcommand, size, command.c_str(), size-1);
    LPWSTR command_line_buf = const_cast<wchar_t*>(wcommand);

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    if (suppressoutput) {
        si.hStdError = g_hChildStd_OUT_Wr;
        si.hStdOutput = g_hChildStd_OUT_Wr;
        si.hStdInput = g_hChildStd_IN_Rd;
        si.dwFlags |= STARTF_USESTDHANDLES;
    }

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
#include <sys/mman.h>
#include <cassert>

using std::cout; using std::endl;
using std::string;

constexpr std::atomic<int> handler_exit_code(103);

std::atomic<int> child_pid;


void SigQuitHandler(int signal_number) {
	assert(signal_number != 0);
    kill(child_pid, SIGTERM);
    while ((child_pid = wait(nullptr)) > 0);
    _exit(handler_exit_code);
}


// Shared memory between main process and fork
static int * sharedreturncode;
const int sharedreturncodeuntouched = 135792468; // Chosen by fair dice roll

pid_t SpawnChild(std::string command) {
    sharedreturncode = static_cast<int *>(mmap(NULL, sizeof *sharedreturncode, PROT_READ | PROT_WRITE,
                                       MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    *sharedreturncode = sharedreturncodeuntouched;

    pid_t ch_pid = fork();
    if (ch_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (ch_pid > 0) {
        return ch_pid;
    } else {
        // Assign the process to a process group that will allow all processes be killed.
        setpgid(getpid(), getpid());

        int returncode = system(command.c_str());
        if (returncode == sharedreturncodeuntouched)
            returncode++;
        *sharedreturncode = returncode;
        exit(returncode);
    }
}

CommandRunner::CommandResult CommandRunner::RunCommand(std::string command, int timeoutms, bool suppressoutput) {
    // In case we are signaled to quit while this is running
    signal(SIGQUIT, SigQuitHandler);

    if (suppressoutput)
    {
        // Pipe output to /dev/null
        command.append(" > /dev/null 2> /dev/null");
    }

    if (timeoutms <= 0) {
        // Regular old system() when no timeout
        return system(command.c_str()) == 0
            ? CommandResult::CommandResultZero
            : CommandResult::CommandResultCodeNonZero;
    }

    // Spawn child
    child_pid = SpawnChild(command);
    int status;

    // A spin wait, possibly something to improve
    while ((waitpid(child_pid, &status, WNOHANG)) == 0 && timeoutms > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));;
        timeoutms--;
    }

    int returncode = *sharedreturncode;
    munmap(sharedreturncode, sizeof *sharedreturncode);

    if (timeoutms <= 0) {
        // Not sure why but you need to add the minus for process group, I guess it is because <-1 means all childs
        kill(-child_pid, SIGKILL);
        return CommandResult::CommandTimedout;
    }

    // Makes sure the shared memory has actually been set
    assert(returncode != sharedreturncodeuntouched);

    return returncode == 0 ?
        CommandResult::CommandResultZero
        : CommandResult::CommandResultCodeNonZero;
}
#endif


