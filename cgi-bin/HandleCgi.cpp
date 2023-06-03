#include "../includes/Cgi/cgi.hpp"

int Response::handle_cgi(Request &request, string file, string exten) {
    Cgi cgi(request, file, exten, *this);

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

    waitpid(0, NULL, 0);
    close(out);

    return 0;
}