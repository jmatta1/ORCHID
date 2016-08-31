/***************************************************************************//**
********************************************************************************
**
** @file floatutil.h
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
** @details Holds definitions of functions for getting info from IEEE 754 floats
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_UTILITY_SPE_FLOATUTIL_H
#define ORCHID_SRC_UTILITY_SPE_FLOATUTIL_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHID


namespace Utility
{

namespace RdwrSpe
{


//a simple negative test for floats this MIGHT be platform specific
//If the platform used the IEEE standard for floats then it should work but
//only the computer gods can tell for certain.
namespace uLPrivate{union floatTester{int i; float f;};}
inline bool isNegFloat(float test)
{
	uLPrivate::floatTester ft;
	ft.f=test;
	return ((ft.i)<0);
}

inline bool isNegZero(float test)
{
	uLPrivate::floatTester ft;
	ft.f=test;
	return (((ft.i&0x7fffffff)==0)&&isNegFloat(test));
}

inline static float negZero()
{
	float out=0;
	return (out=-out);
}

}
}

#endif // ORCHID_SRC_UTILITY_SPE_FLOATUTIL_H
