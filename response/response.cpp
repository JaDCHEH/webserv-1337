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
	string errorpage = _server.get_element("error_page");
	if (code == "301")
		fill_header("Location", _location.get_element("return"));
	std::ifstream file(errorpage);
	file.seekg(0, file.end);
	fill_header("content_type", get_content_type(errorpage.substr(errorpage.find_last_of("."))));
	fill_header("content_lenght", std::to_string(file.tellg()));
	file.seekg(0, file.beg);
	string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	_headers += "\r\n";
	Request._buffer = _initial_line + _headers + file_content;
	send(Request.socket, Request._buffer.c_str(), Request._buffer.size(), 0);
}

void	response::Create_response(Request & Request, config & config, string code)
{
	reset_values();
	_server = config.matchname(Request.host);
	_location = _server.matchlocation(Request.path);
	if (code != "")
		unvalid_response (Request, code);
	else if (_location.get_real() == -1)
		unvalid_response (Request, "404");
	else if (_location.find_element("return"))
		unvalid_response (Request, "301");
	else if (!_location.is_method_allowed(Request.method))
		unvalid_response (Request, "405");
}
