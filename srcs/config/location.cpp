#include "../../includes/Config/Location.hpp"

string	get_words(string &line, stringvect& vector)
{
	string::iterator it = line.begin();
	string word;
	string key;
	int flag = 0;

	vector.clear();
	while (it != line.end())
	{
		word.clear();
		while ((*it == ' ' || *it == '\t') && it != line.end())
			it++;
		while ((*it != ' ' && *it != '\t') && it != line.end())
		{
			if (*it)
				word += (*it);
			it++;
		}
		if (word != "" && flag == 0)
		{
			key = word;
			flag = 1;
		}
		else if (word != "")
			vector.push_back(word);
	}
	return key;
}

string 		Location::get_element(string key)
{
	return(_elements[key]);
}
int	Location::is_method_allowed(string method)
{
	if (std::find(_allowed_methods.begin(), _allowed_methods.end(), method) != _allowed_methods.end())
		return 1;
	return 0;
}

int	Location::find_element(string element)
{
	if (_elements.find(element) != _elements.end())
		return 1;
	return 0;
}

int Location::location_elements(const string &element)
{
	mapstring location_ele;

	location_ele["index"] = "";
	location_ele["auto_index"] = "";
	location_ele["root"] = "";
	location_ele["upload"] = "";
	location_ele["return"] = "";
	location_ele["upload"] = "";
	location_ele["upload_dir"] = "";
	location_ele["cgi"] = ""; // I add CGI to location elemets ;)
	if(location_ele.find(element) != location_ele.end())
		return 1;
	return 0;
}

void	Location::check_validity()
{
	for (stringvect::iterator it = _allowed_methods.begin(); it != _allowed_methods.end(); it++)
	{
		if ((*it) != "GET" && (*it) != "DELETE" && (*it) != "POST")
		{
			std::cerr << (*it) << " Unvalid method in the Configuration file" << std::endl;
			exit(1);
		}
	}
	if (_elements["cgi"] == "on")
		set_CgiFlag(1);
}

void	Location::set_CgiFlag(int a)
{
	_cgiFlag = a;
}

int		Location::get_CgiFlag()
{
	return _cgiFlag;
}

void	Location::set_real(int a)
{
	_real = a;
}

int		Location::get_real()
{
	return _real;
}

void Location::must_fill(const string &root)
{
	if (_elements.find("root") == _elements.end())
		_elements["root"] = root;
	if (_elements.find("auto_index") == _elements.end())
		_elements["auto_index"] = "off";
	if (_elements.find("upload") == _elements.end())
		_elements["upload"] = "off";
	if (_elements.find("upload_dir") == _elements.end())
		_elements["upload_dir"] = "./upload";
	if (_elements["upload_dir"].back() != '/')
		_elements["upload_dir"] += "/";
	if (_elements["root"].back() != '/')
		_elements["root"] += "/";
}

void	Location::location_fill(std::ifstream &ifs, string &line, string &name)
{
	string word;
	stringvect vector;
	int allowed = 0;

	_elements["name"] = name;
	std::getline(ifs, line);
	_cgiFlag = 0;// init cgi flag
	_real = 1;
	word = get_words(line,vector);
	if (word != "{" || vector.size() >= 1)
	{
		std::cerr << "server block is unvalid" << std::endl;
		exit(0);
	}
	while (std::getline(ifs, line))
	{
		word = get_words(line, vector);
		if (word == "allowed_methods")
		{
			if (allowed == 1)
			{
				std::cerr << word <<" is already presented in this block" << std::endl;
				exit(0);
			}
			_allowed_methods = vector;
			allowed = 1;
		}
		else if (location_elements(word) && vector.size() == 1)
		{
			if (_elements.find(word) != _elements.end())
			{
				std::cerr << word <<" is already presented in this block" << std::endl;
				exit(0);
			}
			_elements[word] = vector[0];
		}
		else if (word == "}" && vector.size() == 0)
			break;
		else
		{
			std::cerr << "unvalid element " << word << " or arguments in the location block" << std::endl;
			exit(0);
		}
	}
	if (word != "}") 
	{
		std::cerr << "missing ending for the location block" << std::endl;
		exit(0);
	}
	check_validity();
}

void	Location::reset()
{
	set_real(1);
	_allowed_methods.clear();
	_elements.clear();
}