#include "cgi.hpp"

Cgi::Cgi( Request& request, string file, string extention, Response& res) {
    _file = file;
    _exten = extention;
    _req = request;
	username = getlogin();
	av = new char *[3];
	res.fill_initial_line(request.http_version, "200");
	_init_line = res.get_initial_line();
}

void    Cgi::free_all(char **env, char **av)
{
    for (int i = 0; env[i]; i++)
        delete[] env[i];
    delete[] env;
    for (int j = 0; j < 3; j++)
        free(av[j]);
    delete[] av;
}

char** Cgi::convertMapToCharArray(mapstring& map) {
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

void    Cgi::fill_env( string file, Request &request, mapstring &_env) {
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

void    Cgi::init() {
    // Set environment variables for the CGI script
    fill_env(_file, _req, _env);
	env = convertMapToCharArray(_env);
	if (_exten == ".php")
        execut = "/Users/" + username + "/Desktop/webserv/cgi-bin/php-cgi";
    else
        execut = "/usr/bin/python3";
    av[0] = strdup(execut.c_str());
    av[1] = strdup(_file.c_str());
    av[2] = NULL;
}

char**	Cgi::getEnvir( void ){
	return (env);
}

char** Cgi::getArgv( void ) {
	return (av);
}

Cgi::~Cgi( void ) {
	free_all(env, av);
    std::ifstream input("out");
    std::stringstream sss;
    sss << input.rdbuf();
    resp = _init_line + sss.str();
    send(_req.socket, resp.c_str(), resp.size(), 0);
    unlink("out");
    unlink("in");
}