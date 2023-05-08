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

string	response::error_page_builder(string code)
{
	string page= "<!DOCTYPE html>\n<html>\n  <head>\n	<title>" + code + " - " + _code_map[code]
		+ "</title>\n  </head>\n  <body>\n	<h1>" + code + " - " + _code_map[code] + "</h1>\n  </body>\n</html>";
	return page;
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

string file_get(string File)
{
	std::ifstream file(File);
	string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	return file_content;
}

int	response::simple_response(Request &Request, string code)
{
	string error;
	fill_initial_line(Request.http_version, code);
	if (code == "301")
		fill_header("Location", Request._location.get_element("return"));
	_headers.clear();
	if(Request._server.get_error_page(code) == "")
	{
		fill_header("Content-Type", "text/html");
		error = error_page_builder(code);
	}
	else
	{
		fill_header("Content-Type", get_content_type(Request._server.get_error_page(code).substr(Request._server.get_error_page(code).find_last_of("."))));
		error = file_get(Request._server.get_error_page(code));
	}
	fill_header("Content-Length", std::to_string(error.size()));
	_headers += "\r\n";
	Request._buffer = _initial_line + _headers + error;
	send(Request.socket, Request._buffer.c_str(), Request._buffer.size(), 0);
	return 0;
}



int	response::Create_response(Request & Request, string code)
{
	reset_values();
	if (code != "")
		return simple_response (Request, code);
	else if (Request._location.get_real() == -1)
		return simple_response (Request, "404");
	else if (Request._location.find_element("return"))
		return redirection (Request, 0);
	else if (!Request._location.is_method_allowed(Request.method))
		return simple_response (Request, "405");
	else if (Request.method == "GET")
		return Get_method(Request);
	else if (Request.method == "DELETE")
		return Delete_method(Request);
	else if (Request.method == "POST")
		return Post_method(Request);
	return 0;
}
