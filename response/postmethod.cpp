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
	int fd = open(fullpath.c_str(), O_RDONLY);
	if (fd >= 0)
	{
		close(fd);
		return get_file(Request, fullpath);
	}
	else
		return simple_response(Request, "404");
}