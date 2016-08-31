#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <sstream>

class StringConv
{
public:
	template <class T>
	bool fromString(T &t, const std::string &s, std::ios_base& (*f)(std::ios_base&)=std::dec);
private:
	std::istringstream iss;
};


template <class T>
bool StringConv::fromString(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
{
	iss.str(s);
	iss.clear();
	return !((iss>>f>>t).fail());
}

#endif // STRINGUTIL_H
