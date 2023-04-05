#include "response.hpp"

void response::_codes()
{
	_code_map["501"] = "Not Implemented";
	_code_map["400"] = "Bad Request";
	_code_map["414"] = "Request-URI Too Long";
	_code_map["413"] = "Request Entity Too Large";
	_code_map["404"] = "Not Found";
	_code_map["301"] = "Moved Permanently";
	_code_map["405"] = "Method Not Allowed";
	_code_map["403"] = "Forbidden";
	_code_map["200"] = "OK";
	_code_map["409"] = "Conflict";
	_code_map["204"] = "No Content";
	_code_map["500"] = "Internal Server Error";
}
void	response::_extentions()
{
	_extention_map["text/html"] = ".html";
	_extention_map["text/plain"] = ".txt";
	_extention_map["application/json"] = ".json";
	_extention_map["application/xml"] = ".xml";
	_extention_map["image/gif"] = ".gif";
	_extention_map["application/pdf"] = ".pdf";
	_extention_map["application/zip"] = ".zip";
	_extention_map["application/javascript"] = ".js";
	_extention_map["application/json"] = ".json";
	_extention_map["application/pdf"] = ".pdf";
	_extention_map["audio/webm"] = ".weba";
	_extention_map["image/jpeg"] = ".jpeg";
	_extention_map["image/png"] = ".png";
	_extention_map["image/webp"] = ".webp";
	_extention_map["message/http"] = ".http";
	_extention_map["text/css"] = ".css";
	_extention_map["video/mp4"]= ".mp4";
	_extention_map["video/mpeg"] = ".mpeg";
	_extention_map["application/octet-stream"] = ".bin";
}

string	response::get_content_type(string extention)
{
	for (mapstring::iterator it = _extention_map.begin(); it != _extention_map.end(); it++)
	{
		if (it->second == extention)
			return it->first;	
	}
	return "";
}

string	response::get_extention(string content_type)
{
	mapstring::iterator it = _extention_map.find(content_type);
	if (it != _extention_map.end())
		return it->second;
	return "bin";
}

void	response::reset_values()
{
	_initial_line.clear();
	_headers.clear();
	_body.clear();
	_codes();
	_extentions();
}

void	response::fill_initial_line(const string version, const string _code)
{
	_initial_line = version + " " + _code + " " + _code_map[_code] + "\r\n";
}

void	response::fill_header(const string header, const string value)
{
	_headers += (header + ": " + value + "\r\n");
}

string	&response::get_initial_line()
{
	return _initial_line;
}

string	&response::get_headers()
{
	return _headers;
}

void	response::unvalid_response(Request &Request, string code)
{
	fill_initial_line(Request.http_version, code);
	string errorpage = Request._server.get_element("error_page");
	if (code == "301")
		fill_header("Location", Request._location.get_element("return"));
	std::ifstream file(errorpage);
	file.seekg(0, file.end);
	fill_header("Content-Type", get_content_type(errorpage.substr(errorpage.find_last_of("."))));
	fill_header("Content-Length", std::to_string(file.tellg()));
	file.seekg(0, file.beg);
	string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	_headers += "\r\n";
	Request._buffer = _initial_line + _headers + file_content + "\r\n";
	std::cout << send(Request.socket, Request._buffer.c_str(), Request._buffer.size(), 0) << " " << Request._buffer.size() << std::endl;
}

void	response::redirection(Request & Request, int flag)
{
	fill_initial_line(Request.http_version, "301");
	if (flag == 0)
		fill_header("Location", Request._location.get_element("return"));
	else
		fill_header("Location", Request.path);
	fill_header("Content-Type", "text/html");
	fill_header("Content-Length",0);
	_headers += "\r\n";
	Request._buffer = _initial_line + _headers;
	send(Request.socket, Request._buffer.c_str(), Request._buffer.size(), 0);
}

void	response::valid_response(Request & Request, string code, const string &file)
{
	fill_initial_line(Request.http_version, code);
	fill_header("Content-Type", file.substr(file.find_last_of(".")));
	std::ifstream  file_stream(file);
	char	buffer[3000];
	file_stream.seekg(0, file_stream.end);
	Request._file_size = file_stream.tellg();
	fill_header("Content-Length", std::to_string(file_stream.tellg()));
	file_stream.seekg(0, file_stream.beg);
	file_stream.close();
	_headers += "\r\n";
	if (!Request._first)
	{
		Request._buffer = _initial_line + _headers;
		Request._first = 1;
		Request._fd = open(file.c_str(), O_RDONLY);
	}
	if (!Request._buffer_state)
	{
		int i = read(Request._fd, buffer, 3000);
		Request._buffer += buffer;
		if(!i)
			close(Request._fd);
	}
	size_t a = send(Request.socket,Request._buffer.c_str(), Request._buffer.size(), 0);
	if (a != Request._buffer.size())
	{
		Request._buffer.substr(a);
		Request._buffer_state = 1;
	}
	else
	{
		Request._buffer.clear();
		Request._buffer_state = 0;
	}
}

void	response::Get_method(Request & Request)
{
	Request.path = Request._location.get_element("root") + Request.path;
	DIR *dir = opendir(Request.path.c_str());
	if (dir)
	{
		if (Request.path.back() != '/')
		{
			Request.path += '/';
			redirection(Request, 1);
		}
		else if (Request._location.get_element("index") != "")
		{
			valid_response(Request, "200", Request.path + Request._location.get_element("index"));
		}
	}
}

void	response::Create_response(Request & Request, string code)
{
	reset_values();
	if (code != "")
		unvalid_response (Request, code);
	else if (Request._location.get_real() == -1)
		unvalid_response (Request, "404");
	else if (Request._location.find_element("return"))
		redirection (Request, 0);
	else if (!Request._location.is_method_allowed(Request.method))
		unvalid_response (Request, "405");
	else if (Request.method == "GET")
	{
		Get_method(Request);
	}
}
