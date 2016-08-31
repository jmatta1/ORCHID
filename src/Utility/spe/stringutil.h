/***************************************************************************//**
********************************************************************************
**
** @file onedspectrum.h
** @author James Till Matta
** @date 27 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds a couple silly functions for string conversion
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_UTILITY_SPE_STRINGUTIL_H
#define ORCHID_SRC_UTILITY_SPE_STRINGUTIL_H
// includes for C system headers
// includes for C++ system headers
#include <string>
#include <sstream>
// includes from other libraries
// includes from ORCHID


namespace Utility
{

namespace RdwrSpe
{


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

}
}
#endif // ORCHID_SRC_UTILITY_SPE_STRINGUTIL_H
