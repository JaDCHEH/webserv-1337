#include "../../includes/Response/Response.hpp"

int	Response::Post_method_cgi(Request &Request)
{
	string fullpath = Request.fullpath;
	string exten;
	string temp = Request._location.get_element("index");
	if (temp != "")
	{
		size_t pos = temp.find_last_of(".");
		if (pos != string::npos)
			exten = temp.substr(pos);
	}
	DIR *dir = opendir(fullpath.c_str());
	if (dir)
	{
		closedir(dir);
		if (Request.path.back() != '/')
		{
			Request.path += '/';
			return redirection(Request, 1);
		}
		if (Request._location.get_element("index") != "" && Request._location.get_CgiFlag() && (exten == ".php" || exten == ".py"))
			return handle_cgi(Request, Request.fullpath + Request._location.get_element("index"), exten);
		else
			return simple_Response(Request, "403");
	}
	int fd = open(fullpath.c_str(), O_RDONLY);
	if (fd >= 0)
	{
		string extention;
		if (fullpath.find_last_of(".") != std::string::npos)
			extention = fullpath.substr(fullpath.find_last_of("."));
		else
			extention = "";
		close(fd);
		if (Request._location.get_CgiFlag() && (extention == ".php" || extention == ".py"))
			return handle_cgi(Request, Request.fullpath, extention);
		else
			return simple_Response(Request, "403");
	}
	return simple_Response(Request, "404");
}

int	Response::Post_method(Request &Request)
{
	if (Request._location.get_element("upload") == "on")
	{
		std::srand(time(NULL));
		string name = std::to_string(rand() / 5);
		std::ofstream file(Request._location.get_element("upload_dir")
			+ name + get_extention(Request.getHeader("Content-Type")));
		if (!file)
			return simple_Response(Request, "404");
		file << Request.body;
		file.close();
		return simple_Response(Request, "201");
	}
	else // CGI part // I need to check if file or dir 
		return Post_method_cgi(Request);
}