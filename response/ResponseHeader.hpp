#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP

#include "config/config.hpp"
#include <sstream>

class ResponseHeader {
public:

	void			setAllow(stringvect methods);
	void			setAllow(const string& allow = "");
	void			setContentLength(size_t size);
	void			setContentLocation(const string& path);
	void			setContentType(string type, string path);
	void			setLocation(int code, const string& redirect);

	string		getHeader(size_t size, const string& path, int code, string type, const string& contentLocation);
	string		notAllowed(stringvect methods, const string& path, int code);
	string		writeHeader(void);
	void			setValues(size_t size, const string& path, int code, string type, const string& contentLocation);
	void			initVals(void);
	string		getStatusMessage(int code);

private:
	string					_allow;
	string					_contentLength;
	string					_contentLocation;
	string					_contentType;
	string					_location;
	std::map<int, string>	_errors;

	void						initErrorMap();
};

#endif
