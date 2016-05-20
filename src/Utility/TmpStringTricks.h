/***************************************************************************//**
********************************************************************************
**
** @file TmpStringConcat.h
** @author James Till Matta
** @date 29 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the template system that concatenates strings also has int to
** strings and digit count utilities
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_UTILITY_TMPSTRINGCONCAT_H
#define ORCHID_SRC_UTILITY_TMPSTRINGCONCAT_H

namespace Utility
{

//this is my template metaprogramming string concatenator
//basicly it takes an array of characters and several distinct characters to be
//appended is then keeps stripping characters off the end of the array and
//prepending them to the suffix characters. Once the leading array is empty, it
//uses the endpoint of the recursion to generate an initializer list of chars
//and type casts it to the appropriate const char[] complete with a null
//terminator on the end
template <const char* str, int len, char... suffix>
struct AppendChars
{
    static constexpr const char* value()
        {return AppendChars<str, len-1, str[len-1], suffix...>::value();}
};

//end point of the recursion
template <const char* str,  char... suffix>
struct AppendChars<str, 0, suffix...>
{
    static constexpr const char* value()
        {return (const char[]){suffix..., 0};}
};

//this is similar to the class above, but it handles two character arrays
//plus an optional list of suffix chars. It reduces the second character array
//to a list of chars in the suffix, then passes everything to AppendChars
template <const char* str1, int len1, const char* str2, int len2, char... suffix>
struct AppendStrings
{
    static constexpr const char* value()
        {return AppendStrings<str1, len1, str2, len2-1, str2[len2-1], suffix...>::value();}
};

template <const char* str1, int len1, const char* str2, char... suffix>
struct AppendStrings<str1, len1, str2, 0, suffix...>
{
    static constexpr const char* value()
        {return AppendChars<str1, len1, suffix...>::value();}
};





//This is the recursive int to string used by IntToString
template <int value, char ... suffix>
struct IntToStringRecur
{
    static constexpr const char* toStr()
        {return IntToStringRecur<(value/10), ('0' + (value%10)), suffix...>::value();}
};

template <char ... suffix>
struct IntToStringRecur <0, suffix...>
{
    static constexpr const char* toStr()
        {return (const char[]){suffix..., 0};}
};

//this class converts an integer value to a string
template <int value>
struct IntToString
{
    static constexpr const char* toStr()
        {return IntToStringRecur<(value/10), ('0' + (value%10))>::value();}
};

//this class counts the number of digits in an integer
//this is to be used in tandem with IntToStr so that they can work with the
//AppendChars and AppendStrings stuff above
template <int value, int count>
struct IntDigitCount
{
    static constexpr int digits()
        {return IntDigitCount<(value/10), (count+1)>::digits();}
};

template <int count>
struct IntDigitCount<0, count>
{
    static constexpr int digits()
        {return (count);}
};
}
#endif //ORCHID_SRC_UTILITY_TMPSTRINGCONCAT_H
