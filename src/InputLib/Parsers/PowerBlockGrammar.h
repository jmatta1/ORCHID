/***************************************************************************//**
********************************************************************************
**
** @file PowerBlockGrammar.h
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
** @details holds the grammars necessary for parsing power blocks into
** the general par structure
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_PARSERS_POWERBLOCKGRAMMAR_H
#define ORCHID_SRC_INPUTLIB_PARSERS_POWERBLOCKGRAMMAR_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include <boost/phoenix/bind/bind_member_function.hpp>
// includes from ORCHID
#include"InputLib/Blocks/PowerBlock.h"
#include"Utility.h"

namespace InputParser
{

namespace Parsing
{

namespace qi = boost::spirit::qi;

// the gammar to parse a CalculationBlock
template <typename Iterator>
struct PowerBlockGrammar : qi::grammar<Iterator>
{
	PowerBlockGrammar(PowerBlock* pb) : PowerBlockGrammar::base_type(startRule), ptr(pb)
	{
		namespace phoenix = boost::phoenix;
		using qi::skip;
		using qi::blank;
		using qi::lexeme;
		using qi::float_;
		using qi::int_;
		using qi::eol;
		using Utility::separator;
        
        //define the rules to parse the parameters
        totalChanAvail = (lexeme["TotalChannelsAvailable"]  >> '=' > int_         [phoenix::bind(&PowerBlock::totalChannelsAvailableSet , ptr, qi::_1)] > separator);
        paramFile      = (lexeme["PerChannelParameterFile"] >> '=' > quotedString [phoenix::bind(&PowerBlock::perChannelParameterFileSet, ptr, qi::_1)] > separator);
        
		// define the start rule which holds the whole monstrosity and set the rule to skip blanks
		// if we skipped spaces we could not parse newlines as separators
		startRule = skip(blank) [powerBlockRule];
		powerBlockRule = lexeme["[PowerBlock]"] >> *eol
                               > ( 
                                   totalChanAvail ^ paramFile
                                 )
                               > lexeme["[EndBlock]"];
	}

private:
    //base rules for the file
    qi::rule<Iterator>                 startRule;
	qi::rule<Iterator, qi::blank_type> powerBlockRule;
	
	// special sub grammars
	Utility::QuotedString<Iterator> quotedString;
	
	// parameters
	qi::rule<Iterator, qi::blank_type> totalChanAvail, paramFile;

	// hold the pointer that we are going to bind to
	PowerBlock* ptr;
};

}

}

#endif  // ORCHID_SRC_INPUTLIB_PARSERS_POWERBLOCKGRAMMAR_H
