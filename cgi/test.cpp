#include "../config/Location.hpp"

int execute_cgi(char **av)
{
    int fd[2];
    int f;
    int tmp_fd;
    char *Env[6] = {"CONTENT_TYPE=text/html", "CONTENT_LENGTH=1239",
                    "QUERY_STRING", "REQUEST_METHOD=GET"
                    "SCRIPT_FILENAME=test.php", "REDIRECT_STATUS=200"};

    tmp_fd = dup(0);
    pipe(fd);
    f = fork();
    if (f == 0) {
        dup2(fd[1], 1);
        dup2(tmp_fd, 0);
        close(fd[1]);
        close(tmp_fd);
       if (execve(av[0], av, Env)) {
            std::cout << "execve() failed." << std::endl;
            return (1);
        }
    }
    else {
        close(tmp_fd);
        close(fd[1]);
        tmp_fd = fd[0];
    }
    return (0);
}


int main(int ac, char **av)
{
    execute_cgi(av);
}