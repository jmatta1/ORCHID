/***************************************************************************//**
********************************************************************************
**
** @file utility.h
** @author James Till Matta
** @date 26 Mar, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Header to hold utility classes and definitions for input file
** parsing
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_PARSERS_UTILITY_H
#define ORCHID_SRC_INPUTLIB_PARSERS_UTILITY_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include <boost/spirit/include/qi.hpp>
// includes from ORCHID

namespace InputParser
{

namespace Utility
{

//symbol table for reading in boolean values
struct BoolSymbols_ : boost::spirit::qi::symbols<char, bool>
{
    BoolSymbols_(){
        this->add("True" , true )
                 ("true" , true )
                 ("T"    , true )
                 ("t"    , true )
                 ("False", false)
                 ("false", false)
                 ("F"    , false)
                 ("f"    , false);
     }
};

static const BoolSymbols_ boolSymbols_;

// define my 'line enders' here, use auto cause it is going to return a strange templated type
// this version is for use within blocks, it allows key value pairs to be separated with
// commas, newlines, or comments followed by newlines
static const auto separator = boost::spirit::qi::copy(',' | +(boost::spirit::qi::eol | ('#' >> *(boost::spirit::qi::char_ - boost::spirit::qi::eol) >> boost::spirit::qi::eol)));
	//allows trailing comments newlines comment lines in any order and quantity
// this line ender version is a more strict newline in that it requires a newline character
static const auto eol_ = boost::spirit::qi::copy( +(('#' >> *(boost::spirit::qi::char_ - boost::spirit::qi::eol) >> boost::spirit::qi::eol) | boost::spirit::qi::eol));

//grammar for parsing and returning quoted strings
template <typename Iterator>
struct QuotedString : public boost::spirit::qi::grammar<Iterator, std::string()>
{
    QuotedString() : QuotedString::base_type(quotedStringRule)
    {
        quotedStringRule = '"' > +~boost::spirit::qi::char_("\"\n") > '"';
    }
private:
    boost::spirit::qi::rule<Iterator, std::string()> quotedStringRule;
};

}

}

#endif  // ORCHID_SRC_INPUTLIB_PARSERS_UTILITY_H
