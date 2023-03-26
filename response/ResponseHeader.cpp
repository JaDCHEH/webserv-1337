
#include "ResponseHeader.hpp"


string		ResponseHeader::getHeader(size_t size, const string& path, int code, string type, const string& contentLocation)
{
	string	header;

	initVals();
	setValues(size, path, code, type, contentLocation);

	header = "HTTP/1.1 " + std::to_string(code) + " " + getStatusMessage(code) + "\r\n";
	header += writeHeader();

	return (header);
}

string		ResponseHeader::notAllowed(stringvect methods, const string& path, int code)
{
	string	header;

	initVals();
	setValues(0, path, code, "", path);
	setAllow(methods);

	if (code == 405)
		header = "HTTP/1.1 405 Method Not Allowed\r\n";
	else if (code == 413)
		header = "HTTP/1.1 413 Payload Too Large\r\n";
	header += writeHeader();

	return (header);
}

string		ResponseHeader::writeHeader(void)
{
	string	header = "";

	if (_allow != "")
		header += "Allow: " + _allow + "\r\n";
	if (_contentLength != "")
		header += "Content-Length: " + _contentLength + "\r\n";
	if (_contentLocation != "")
		header += "Content-Location: " + _contentLocation + "\r\n";
	if (_contentType != "")
		header += "Content-Type: " + _contentType + "\r\n";
	return (header);
}

string		ResponseHeader::getStatusMessage(int code)
{
	if (_errors.find(code) != _errors.end())
		return _errors[code];
	return ("Unknown Code");
}

void			ResponseHeader::initErrorMap()
{
	_errors[100] = "Continue";
	_errors[200] = "OK";
	_errors[201] = "Created";
	_errors[204] = "No Content";
	_errors[400] = "Bad Request";
	_errors[403] = "Forbidden";
	_errors[404] = "Not Found";
	_errors[405] = "Method Not Allowed";
	_errors[413] = "Payload Too Large";
	_errors[500] = "Internal Server Error";
}

void			ResponseHeader::setValues(size_t size, const string& path, int code, string type, const string& contentLocation)
{
	setAllow();
	setContentLength(size);
	setContentLocation(contentLocation);
	setContentType(type, path);
	setLocation(code, path);
}

void			ResponseHeader::initVals(void)
{
	_allow = "";
	_contentLength = "";
	_contentLocation = "";
	_contentType = "";
	_location = "";
	initErrorMap();
}

void			ResponseHeader::setAllow(stringvect methods)
{
	stringvect::iterator it = methods.begin();

	while (it != methods.end())
	{
		_allow += *(it++);

		if (it != methods.end())
			_allow += ", ";
	}
}

void			ResponseHeader::setAllow(const string& allow)
{
	_allow = allow;
}

void			ResponseHeader::setContentLength(size_t size)
{
	_contentLength = std::to_string(size);
}

void			ResponseHeader::setContentLocation(const string& path)
{
	_contentLocation = path;
}

void			ResponseHeader::setContentType(string type, string path)
{
	if (type != "")
	{
		_contentType = type;
		return ;
	}
	type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));
	if (type == "html")
		_contentType = "text/html";
	else if (type == "jpeg" || type == "jpg")
		_contentType = "image/jpeg";
	else if (type == "png")
		_contentType = "image/png";
}


void			ResponseHeader::setLocation(int code, const string& redirect)
{
	if (code == 201 || code / 100 == 3)
	{
		_location = redirect;
	}
}
