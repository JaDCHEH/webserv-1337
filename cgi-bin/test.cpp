#include "../response/response.hpp"

#define MAX_INPUT 1024

void    set_env(mapstring mas)
{
    string test = "";

    for (mapstring::iterator it = mas.begin(); it != mas.end(); it++)
    {
        if ((*it).first == "REQUEST_METHOD")
            std::cout << (*it).first << " " << (*it).second << std::endl;
        else if ((*it).first == "QUERY_STRING")
            std::cout << (*it).first << " " << (*it).second << std::endl;
        else if ((*it).first == "Referer")
            test = (*it).second;
    }
    if (test != "")
        std::cout << "fawsss" << test << std::endl;
}

string response::handle_cgi(Request &request, string file) {
    char buf[1024];
    string resp;
    char envp[MAX_INPUT];
    int fd[2];
    char *env[] = { envp, NULL };
    // char input[MAX_INPUT];
    char* argv[] = {(char *)file.c_str(), NULL };
    // Set environment variables for the CGI script
    set_env(request.headers);
    sprintf(envp, "REQUEST_METHOD=%s", request.method.c_str());
    putenv(envp);
    if (strcasecmp(request.method.c_str(), "GET") == 0) {
        sprintf(envp, "QUERY_STRING=%s", request.query_str.c_str());
        putenv(envp);
    }

    // create a new process to execute the CGI script
    int pid = fork();
    pipe(fd);
    if (pid == 0) {
        dup2(fd[0], 0);
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        execve("./cgi-bin/php-cgi", argv, env);
        // if execve returns, it means there was an error
        perror("execve failed");
        exit(1); 
    }
    else { // parent process
        // wait for the child process to finish
        close(fd[1]);
        close(fd[0]);
        int status;
        int recv;
        recv = read(fd[0], &buf, 1024);
        while(recv > 0)
        {
            std::cout << "asdfasdf : " << buf << std::endl;
            resp += buf;
            recv = read(fd[1], &buf, 1024);
        }
        if (WIFEXITED(status)) {
            // int exit_status = WEXITSTATUS(status);
            // process the exit status if necessary
        }
    }
    return resp;
}