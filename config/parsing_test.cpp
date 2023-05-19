#include "Server.hpp"
#include "../response/response.hpp"

size_t convert(string num) {
    int len = num.length();
    int base = 1;
    int temp = 0;
    for (int i = len - 1; i >= 0; i--) {
        if (num[i] >= '0' && num[i] <= '9') {
            temp += (num[i] - '0') * base;
            base = base * 16;
        }
        else if (num[i] >= 'a' && num[i] <= 'f') {
            temp += (num[i] - 'a' + 10) * base;
            base = base * 16;
        }
        else
            return (0);
    }
    return temp;
}

void	Parsing_test(string request)
{
	string new_req = request.substr(request.find("\r\n\r\n") + 4);
	std::istringstream iss(new_req);
    string s;
    string str;
	size_t str_length;
    while (std::getline(iss, str, '\r')) {
        iss.ignore(1); // to ignore "\n"
		if (str[0] == '0') // 0 means no more line to read
			break ;
		str_length = convert(str);// convert length from hex to decimal
        char *body = new char[str_length];
		iss.read(body, str_length);
        string dataString(body, str_length);
		s += dataString;
        delete[] body;
    }
}

int main()
{
	std::string request =   "POST /RegisterStudent.asp HTTP/1.1\r\n"
                            "Host: guru99.co\r\n"
                            "User-Agent: Mozilla/5.0\r\n"
                            "Accept: text/xml, text/html, text/plain, image/ipe\r\n"
                            "Accept-Language: en-us,e\r\n"
                            "Accept-Encoding: gzip, deflate\r\n"
                            "Accept-Charset: ISO-8859-1, utf-8\r\n"
                            "Connection: keep-alive\r\n"
                            "Keep-Alive: 30\r\n"
                            "Transfer-Encoding: chunked\r\n"
                            "\r\n"
                            "b\r\n"
                            "Hello World!\r\n"
                            "1c\r\n"
                            "consectetur adipiscing elit.\r\n"
                            "0\r\n"
                            "\r\n";
	
	Parsing_test(request);
	return (0);
}
