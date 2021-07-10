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
        cout << "spawn child with pid - " << ch_pid << endl;
        return ch_pid;
    } else {
	    setpgid(getpid(), getpid()); // Assign the process to a process group that will allow all processes be killed.
        int returncode = system(command.c_str());
        std::cout << "Return code: " << returncode << std::endl;
        exit(returncode);
    }
}

int RunCommand(std::string command, int timeoutms) {
    // In case we are signaled to quit while this is running
    signal(SIGQUIT, SigQuitHandler);

    if (timeoutms <= 0) {
        // Regular old system() when no timeout
        return (system(command.c_str()));
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
	    std::cout << "Killing process..." << std::endl;
        kill(-child_pid, SIGKILL);
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        int signum = WTERMSIG(status);
        printf("Exited due to receiving signal %d\n", signum);
    } else if (WIFSTOPPED(status)) {
        int signum = WSTOPSIG(status);
        printf("Stopped due to receiving signal %d\n", signum);
    } else {
        printf("Something strange just happened.\n");
    }

    while ((child_pid = wait(nullptr)) > 0)
        cout << "child " << child_pid << " terminated" << endl;

    return WEXITSTATUS(status);
}

int main() {
    string program_name("echo \"Hello\"");
    std::cout << "RunCommand: " << RunCommand("while true ; do echo xx ; sleep 1; done", 10000) << std::endl;
    return 0;
    //char * const *arg_list[] = {program_name.data(), nullptr};


    //children = reinterpret_cast<std::atomic<int> *>(new int[FORK_NUM]);

    //signal(SIGQUIT, sigquitHandler);

    int child_pid = SpawnChild(program_name);
    //sleep(10);
    cout << endl;
    if (WIFSTOPPED(child_pid) == 0) {
        std::cout << "Child " << child_pid << " is still running" << std::endl;
    }

    int status;
    if ((waitpid(child_pid, &status, WNOHANG)) > 0) {
        cout << "child " << child_pid << " terminated1" << endl;
    } else {
        cout << "child waited for" << endl;
    }
    sleep(5);
    if ((waitpid(child_pid, &status, WNOHANG)) > 0) {
        cout << "child " << child_pid << " terminated2" << endl;
    } else {
        cout << "child waited for" << endl;
        cout << "killing chinld.." << endl;
        kill(child_pid, SIGTERM);
        kill(child_pid, SIGTERM);
    }
    if (WIFSTOPPED(child_pid) == 0 && WIFEXITED(child_pid) == 0) {
        std::cout << "Child is still running" << std::endl;
    } else {
        std::cout << "Child killed" << std::endl;
    }

    while ((child_pid = wait(nullptr)) > 0)
        cout << "child " << child_pid << " terminated" << endl;

    printf("exited, status=%d\n", WEXITSTATUS(status));
    return EXIT_SUCCESS;
}
