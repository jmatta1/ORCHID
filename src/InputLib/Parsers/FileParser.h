/***************************************************************************//**
********************************************************************************
**
** @file FileParser.h
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
** @details Holds the overall file parser used to parse a file full of blocks
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_PARSERS_FILEPARSER_H
#define ORCHID_SRC_INPUTLIB_PARSERS_FILEPARSER_H

// includes for C system headers
// includes for C++ system headers
#include<string>
#include<iostream>
// includes from other libraries
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
// includes from ORCHID
#include"InputLib/Blocks/InputParams.h"
#include"GeneralBlockGrammar.h"
#include"DigitizerBlockGrammar.h"
#include"PowerBlockGrammar.h"

namespace InputParser 
{

namespace Parsing
{

template <typename Iterator>
struct InputFileParser : qi::grammar<Iterator>
{
    InputFileParser(InputParameters* params) :
        InputFileParser::base_type(start),
        generalBlockGrammar(params->generalBlock),
        digitizerBlockGrammar(params->digitizerBlock),
        powerBlockGrammar(params->powerBlock)
	{
		using qi::skip;
		using qi::eol;
		using qi::eoi;
		using qi::blank;
		using qi::lexeme;
		using qi::fail;
		using qi::on_error;
		namespace phoenix = boost::phoenix;
        
		// define the start rule which holds the whole monstrosity and set the rule to skip blanks
		// if we skipped spaces we could not parse newlines as separators
		start    = skip(blank) [fileRule];
        fileRule = lexeme["[Start]"] >> +eol
                   > (
                       ( generalBlockGrammar   > +eol ) ^
                       ( digitizerBlockGrammar > +eol ) ^
                       ( powerBlockGrammar     > +eol )
                     )
                   > lexeme["[End]"] >> *eol >> eoi;
        on_error<fail>
        (
            start,
            std::cout << phoenix::val("Error! Expecting ")
                      << qi::_4                             // what failed?
                      << phoenix::val(" here: \n\"")
                      << phoenix::construct<std::string>(qi::_3, qi::_2) // iterators to error-pos, end
                      << phoenix::val("\"")
                      << std::endl
        );
	}
	
	// main rules
    qi::rule<Iterator>                 start;
    qi::rule<Iterator, qi::blank_type> fileRule;
    // grammars for blocks
    Parsing::GeneralBlockGrammar<Iterator>     generalBlockGrammar;
    Parsing::DigitizerBlockGrammar<Iterator> digitizerBlockGrammar;
    Parsing::PowerBlockGrammar<Iterator>         powerBlockGrammar;
};

}

}

#endif  // ORCHID_SRC_INPUTLIB_PARSETEST_H
