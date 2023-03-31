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
void	response::reset_values()
{
	_initial_line = "";
	_headers = "";
	_status_code = -1;
	_codes();
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

void	response::Create_response(request & request, config & config)
{
	_server = config.matchname(request.host);
	_location = _server.matchlocation(request.uri);
	if (_location.get_real() == -1)
	{
		fill_initial_line(request.version, "404");
		return;
	}
	if (_location.find_element("return"))
	{
		fill_initial_line(request.version, "301");
		fill_header("Location", _location.get_element("return"));
		return;
	}
	if (!_location.is_method_allowed(request.METHOD))
	{
		fill_initial_line(request.version, "405");
		return;
	}
}