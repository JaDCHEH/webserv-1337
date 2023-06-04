#include "../includes/Cgi/cgi.hpp"
#include <sys/wait.h>
#include <signal.h>

int Response::handle_cgi(Request &request, string file, string exten) {
    Cgi cgi(request, file, exten, *this);
    int status;

    cgi.init();
    if (request.method == "POST")
    {
        std::ofstream in("in");
        in.write(request.body.c_str(), request.body.size());
    }
    // create a new process to execute the CGI script
    int out = open("out", O_WRONLY | O_CREAT , 0755);
    int pid = fork();
    if (pid < 0)
    {
        cgi._flag = false;
        return (simple_Response(request, "500"));
    }
    if (pid == 0) {
        if (request.method == "POST")
        {
            int in = open("in", O_RDONLY);
            dup2(in, 0);
            close(in);
        }
        dup2(out, 1); 
        close(out);
        execve(cgi.getArgv()[0], cgi.getArgv(), cgi.getEnvir());
        // if execve returns, it means there was an error
        perror("execve failed");
        exit(1);
    }
    sleep(1);
    int wait_ret = waitpid(0, &status, WNOHANG);
    if (wait_ret <= 0)
    {
        sleep(5);
        wait_ret = waitpid(0, &status, WNOHANG);
        if (wait_ret <= 0)
        {
            kill(pid,SIGTERM);
            cgi._flag = false;
            return (simple_Response(request, "501"));
        }
    }
    if (WIFEXITED(status))
    {
        if (WEXITSTATUS(status) == 1)
        {
            close(out);
            unlink("out");
            unlink("in");
            // cgi.free_all();
            return (simple_Response(request, "500"));
        }
    }
    close(out);

    return 0;
}