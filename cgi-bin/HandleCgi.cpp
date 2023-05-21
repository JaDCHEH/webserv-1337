#include "cgi.hpp"

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
    std::ifstream Myfile(file);
    if (Myfile)
    {
        string filename = file.substr(file.rfind('/') + 1);
        _env["SCRIPT_NAME"] = filename;
    }

    _env["REDIRECT_STATUS"]= "200";
    _env["REQUEST_METHOD"] = request.method;
    _env["CONTENT_TYPE"] = "text/html";
    _env["CONTENT_LENGTH"] = request._amount_written;
    _env["SCRIPT_FILENAME"] = file;
    if (request.method == "GET") {
        _env["QUERY_STRING"] = request.query_str;
    }
}


int response::handle_cgi(Request &request, string file) {
    char buf[1024];
    mapstring   _env;
    string resp, execut = request._location.get_element("root") + "/cgi-bin/php-cgi";
    int fd[2];
    char **env;
    char **argv = new char *[3];

    if (request.method == "POST")
    {
        std::ofstream   BodyFile("body");
        BodyFile << request.body;
    }
    // Set environment variables for the CGI script
    fill_env(file, request, _env);
    env = convertMapToCharArray(_env);
    argv[0] = strdup(execut.c_str());
    argv[1] = strdup(file.c_str());
    argv[2] = NULL;

    // create a new process to execute the CGI script
    pipe(fd);
    int pid = fork();
    if (pid == 0) {

        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execve(argv[0], argv, env);
        // if execve returns, it means there was an error
        perror("execve failed");
        exit(1); 

    }
    wait(NULL);
    close(fd[1]);
    resp += "HTTP/1.1 200OK";
    while(read(fd[0], &buf, 1024) > 0)
        resp += buf;
    send(request.socket, resp.c_str(), resp.size(), 0);
    return 0;
}