/***************************************************************************//**
********************************************************************************
**
** @file DigitizerChannelParser.h
** @author James Till Matta
** @date 01 June, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details file with the parser grammar to parser arbitrary csv files
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_PARSERS_DIGITIZERCHANNELPARSER_H
#define ORCHID_SRC_INPUTLIB_PARSERS_DIGITIZERCHANNELPARSER_H


// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
// includes from ORCHID
#include"InputLib/Blocks/DigitizerChannelData.h"
#include"Utility.h"

namespace InputParser 
{

namespace Parsing
{

namespace qi = boost::spirit::qi;

template <typename Iterator>
struct DigitizerChannelParser : qi::grammar<Iterator>
{
    DigitizerChannelParser(DigitizerChannelData* data) :
        DigitizerChannelParser::base_type(start),
        ptr(data)
	{
		using qi::skip;
		using qi::eoi;
		using qi::blank;
		using qi::lexeme;
		using qi::fail;
		using qi::on_error;
		using qi::int_;
        using qi::hex;
		using qi::float_;
		using Utility::boolSymbols_;
		using Utility::eol_;
		namespace phoenix = boost::phoenix;
        
		// define the start rule which holds the whole monstrosity and set the rule to skip blanks
		// if we skipped spaces we could not parse newlines as separators
		start    = skip(blank) [fileRule];
        fileRule = (
                     *eol_
                     > lineRule
                     > *(+eol_ >> lineRule)
                     > *eol_
                     > eoi
                   );

        lineRule = ( int_         [phoenix::bind(&DigitizerChannelData::addModuleNumber,           ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerChannelData::addChannelNumber,          ptr, qi::_1)] > lexeme[','] >
                     boolSymbols_ [phoenix::bind(&DigitizerChannelData::addChannelEnable,          ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerChannelData::addParticipateInTrigger,   ptr, qi::_1)] > lexeme[','] >
                     boolSymbols_ [phoenix::bind(&DigitizerChannelData::addLargeRange,             ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerChannelData::addPulseWidth,             ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerChannelData::addTrigThreshold,          ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerChannelData::addCoupleTrigLogic,        ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerChannelData::addCouplePulseType,        ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerChannelData::addDcOffset,               ptr, qi::_1)] > lexeme[',']);

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
    qi::rule<Iterator>                  start;
    qi::rule<Iterator, qi::blank_type > fileRule;
    qi::rule<Iterator, qi::blank_type > lineRule;
    
    //data holder
    DigitizerChannelData* ptr;
};

}

}

#endif //ORCHID_SRC_INPUTLIB_PARSERS_DIGITIZERMODULEPARSER_H
