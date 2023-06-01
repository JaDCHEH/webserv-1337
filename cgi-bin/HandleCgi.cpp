#include "cgi.hpp"

void    free_all(char **env, char **av)
{
    for (int i = 0; env[i]; i++)
        delete[] env[i];
    delete[] env;
    for (int j = 0; j < 3; j++)
        free(av[j]);
    delete[] av;
}

char** convertMapToCharArray(mapstring& map) {
    // Create a char** array to hold the converted data
    char** charArray = new char*[map.size() + 1]; // +1 for the terminating nullptr
    
    // Convert each key-value pair to "KEY=VALUE" format
    size_t i = 0;
    for (mapstring::iterator iter = map.begin(); iter != map.end(); ++iter) {
        string envVar = iter->first + "=" + iter->second;
        // Allocate memory for the current environment variable
        charArray[i] = new char[envVar.size() + 1];
        // Copy the environment variable string to charArray
        std::strcpy(charArray[i], envVar.c_str());
        ++i;
    }
    // Set the last element of the array as nullptr
    charArray[map.size()] = NULL;
    return charArray;
}

void    fill_env( string file, Request &request, mapstring &_env) {
    _env["REDIRECT_STATUS"]= "200";
    _env["REQUEST_METHOD"] = request.method;
    if (request.method == "POST")
    {
        _env["CONTENT_TYPE"] = request.getHeader("Content-Type");
        _env["CONTENT_LENGTH"] = request.getHeader("Content-Length");
    }
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["DOCUMENT_ROOT"] = "../";
    _env["PATH_INFO"] = file;
    _env["SCRIPT_NAME"] = file;
    _env["SCRIPT_FILENAME"] = file;
    if (request.method == "GET")
        _env["QUERY_STRING"] = request.query_str;
    _env["HTTP_COOKIE"] = request.getHeader("Cookie");
}

int Response::handle_cgi(Request &request, string file, string exten) {
    string buf;
    mapstring   _env;
    string resp, execut;
    char **env;
    char **argv = new char *[3];

    // Set environment variables for the CGI script
    fill_env(file, request, _env);
    env = convertMapToCharArray(_env);
    if (exten == ".php")
        execut = "/Users/cjad/Desktop/webserv/cgi-bin/php-cgi";
    else
        execut = "/usr/bin/python3";
    argv[0] = strdup(execut.c_str());
    argv[1] = strdup(file.c_str());
    argv[2] = NULL;
    if (request.method == "POST")
    {
        std::ofstream in("in");
        in.write(request.body.c_str(), request.body.size());
    }
    // create a new process to execute the CGI script
    int out = open("out", O_WRONLY | O_CREAT , 0755);
    int pid = fork();
    if (pid == 0) {
        if (request.method == "POST")
        {
            int in = open("in", O_RDONLY);
            dup2(in, 0);
            close(in);
        }
        dup2(out, 1); 
        close(out);
        // for (int i = 0; env[i]; i++)
        //     std::cerr << "env -> " << env[i] << std::endl;
        execve(argv[0], argv, env);
        // if execve returns, it means there was an error
        perror("execve failed");
        exit(1);
    }
    waitpid(0, NULL, 0);
    close(out);
    std::ifstream input("out");
    std::stringstream sss;
    fill_initial_line(request.http_version, "200");
    sss << input.rdbuf();
    resp = _initial_line + sss.str();
    send(request.socket, resp.c_str(), resp.size(), 0);
    unlink("out");
    unlink("in");
    return 0;
}