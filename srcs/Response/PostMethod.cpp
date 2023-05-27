#include "../../includes/Response/Response.hpp"

int	Response::Post_method(Request &Request)
{
	if (Request._location.get_element("upload") == "on")
	{
		if (Request.getHeader("File-Name") != "")
		{
			std::ofstream file(Request._location.get_element("upload_dir")
				+ Request.getHeader("File_Name") + get_extention(Request.getHeader("Content-Type")));
			file << Request.body;
			return simple_Response(Request, "201");
		}
		else
		{
			std::srand(time(NULL));
			string name = std::to_string(rand() / 5);
			std::ofstream file(Request._location.get_element("upload_dir")
				+ name + get_extention(Request.getHeader("Content-Type")));
			file << Request.body;
			return simple_Response(Request, "201");
		}
	}
	else // CGI part // I need to check if file or dir 
		return handle_cgi(Request, Request._location.get_element("root") + Request.path + Request._location.get_element("index"));
	return (0);
}