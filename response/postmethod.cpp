#include "response.hpp"

int	response::Post_method(Request &Request)
{
	if (Request._location.get_element("upload") == "on")
	{
		if (Request.getHeader("File-Name") != "")
		{
			std::ofstream file(Request._location.get_element("upload_dir")
				+ Request.getHeader("File_Name") + get_extention(Request.getHeader("Content-Type")));
			file << Request.body;
			return simple_response(Request, "201");
		}
		else
		{
			std::srand(time(NULL));
			string name = std::to_string(rand() / 5);
			std::ofstream file(Request._location.get_element("upload_dir")
				+ name + get_extention(Request.getHeader("Content-Type")));
			file << Request.body;
			return simple_response(Request, "201");
		}
	}
	else 
	{
		string fullpath = Request._location.get_element("root") + Request.path;
//		DIR *dir = opendir(fullpath.c_str());
		std::ofstream file(fullpath);
		if (!file)
			return simple_response(Request, "403");
		file << Request.body;
		return simple_response(Request, "201");
	}
}