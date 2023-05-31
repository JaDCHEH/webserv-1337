#include "../../includes/Response/Response.hpp"

int	Response::Post_method_cgi(Request &Request)
{
	string fullpath = Request.fullpath;
	DIR *dir = opendir(fullpath.c_str());
	if (Request._location.get_CgiFlag() && dir)
		return handle_cgi(Request, fullpath + Request._location.get_element("index"));
	else if (Request._location.get_CgiFlag())
		return handle_cgi(Request, fullpath);
	if (dir)
	{
		closedir(dir);
		if (Request.path.back() != '/')
		{
			Request.path += '/';
			return redirection(Request, 1);
		}
		if (Request._location.get_element("index") != "" && Request._location.get_CgiFlag())
			return handle_cgi(Request, Request.fullpath + Request._location.get_element("index"));
		else
			return simple_Response(Request, "403");
	}
	int fd = open(fullpath.c_str(), O_RDONLY);
	if (fd >= 0)
	{
		close(fd);
		if (Request._location.get_CgiFlag())
			return handle_cgi(Request, Request.fullpath);
		else
			return simple_Response(Request, "403");
	}
	return simple_Response(Request, "404");
}

int	Response::Post_method(Request &Request)
{
	if (Request._location.get_element("upload") == "on")
	{
		if (Request.getHeader("File-Name") != "")
		{
			std::ofstream file(Request._location.get_element("upload_dir")
				+ Request.getHeader("File_Name") + get_extention(Request.getHeader("Content-Type")));
			if (!file)
				return simple_Response(Request, "404");
			file << Request.body;
		}
		else
		{
			std::srand(time(NULL));
			string name = std::to_string(rand() / 5);
			std::ofstream file(Request._location.get_element("upload_dir")
				+ name + get_extention(Request.getHeader("Content-Type")));
			if (!file)
				return simple_Response(Request, "404");
			file << Request.body;
		}
		return simple_Response(Request, "201");
	}
	else // CGI part // I need to check if file or dir 
		return Post_method_cgi(Request);
}