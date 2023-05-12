#include "response.hpp"

int	response::Post_method(Request &Request)
{
	string fullpath = Request._location.get_element("root") + Request.path;
	DIR *dir = opendir(fullpath.c_str());
	if (dir)
	{
		closedir(dir);
		if (fullpath.back() != '/')
		{
			Request.path += '/';
			return redirection(Request, 1);
		}
	}
	else
	{
		std::ofstream file(fullpath);
		if (!file)
			return simple_response(Request, "403");
		file << Request.body;
		return simple_response(Request, "201");
	}
}