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


// void	setEnvVars(std::pair<int, Request>& request, Config& conf, string method, std::map<std::string, std::string> &myMap, string& cgiFile)
// {
// 	string cookies = request.second.getInfo("cookies").substr(0, request.second.getInfo("cookies").find_first_of("\r"));
// 	if (method == "POST")
// 	{
// 		string CL = request.second.getInfo("content length").substr(0, request.second.getInfo("content length").find_first_of("\r"));
// 		string CT = request.second.getInfo("content type").substr(0, request.second.getInfo("content type").find_first_of("\r"));
// 		myMap.insert(std::make_pair("CONTENT_LENGTH", CL));
// 		myMap.insert(std::make_pair("CONTENT_TYPE", CT));
		
// 	}
// 	if (method == "GET")
// 	{
// 		string url = request.second.getInitialLine()[1];
// 		size_t pos = 0;
// 		pos = url.find("?", pos);
// 		if (pos == std::string::npos){
// 			myMap.insert(std::make_pair("QUERY_STRING", ""));
// 		} else {
// 			if (url[url.length() - 1] == '/')
// 				url.erase(url.end());
// 			url = url.substr(pos+1);
// 			myMap.insert(std::make_pair("QUERY_STRING", url));
// 		}
// 	}
	// myMap.insert(std::make_pair("REQUEST_METHOD", method));
	// myMap.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	// myMap.insert(std::make_pair("SERVER_NAME", conf.getServers()[request.second.getIdentifier("server")].listen.second.first));
	// myMap.insert(std::make_pair("SERVER_PORT", conf.getServers()[request.second.getIdentifier("server")].listen.second.second));
	// myMap.insert(std::make_pair("SCRIPT_NAME", cgiFile));
	// myMap.insert(std::make_pair("SCRIPT_FILENAME", cgiFile)); 
	// myMap.insert(std::make_pair("PATH_INFO", cgiFile)); 
	// myMap.insert(std::make_pair("DOCUMENT_ROOT", "../"));
	// myMap.insert(std::make_pair("HTTP_HOST", conf.getServers()[request.second.getIdentifier("server")].listen.second.first));
	// myMap.insert(std::make_pair("REDIRECT_STATUS", "200"));
	// myMap.insert(std::make_pair("HTTP_COOKIE", cookies));
// }



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
    // we need to add cookies
}


int Response::handle_cgi(Request &request, string file) {
    char buf[1024];
    mapstring   _env;
    // int     open_file;
    string resp, execut = request._location.get_element("root") + "/cgi-bin/php-cgi";
    char **env;
    char **argv = new char *[3];

    // Set environment variables for the CGI script
    std::cout << "/************************************/\n" << "request : " << request._req << std::endl;
    std::cout << "/************************************/" << std::endl;
    fill_env(file, request, _env);
    env = convertMapToCharArray(_env);
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
    int outpoun = open("out", O_RDWR, 0644);
    fill_initial_line(request.http_version, "200");
    resp += _initial_line;
    while(read(outpoun, &buf, 1024) > 0)
        resp += buf;
    send(request.socket, resp.c_str(), resp.size(), 0);
    unlink("out");
    unlink("in");
    return 0;
}