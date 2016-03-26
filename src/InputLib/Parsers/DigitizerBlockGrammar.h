/***************************************************************************//**
********************************************************************************
**
** @file DigitizerBlockGrammar.h
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
** @details holds the grammars necessary for parsing digitizer blocks into
** the digitizer block structure
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_PARSERS_DIGITIZERBLOCKGRAMMAR_H
#define ORCHID_SRC_INPUTLIB_PARSERS_DIGITIZERBLOCKGRAMMAR_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include <boost/phoenix/bind/bind_member_function.hpp>
// includes from ECHOS
#include"InputLib/Blocks/DigitizerBlock.h"
#include"utility.h"

namespace InputParser
{

namespace Parsing
{

namespace qi = boost::spirit::qi;

// the main gammar
template <typename Iterator>
struct DigitizerBlockGrammar : qi::grammar<Iterator>
{
	DigitizerBlockGrammar(DigitizerBlock* db) : DigitizerBlockGrammar::base_type(startRule), ptr(db)
	{
		namespace phoenix = boost::phoenix;
		using qi::skip;
		using qi::blank;
		using qi::lexeme;
		using qi::float_;
		using qi::int_;
		using qi::eol;
		using Utility::eol_;
        
        //define the rules to parse the parameters
        totalChanAvail = (lexeme["TotalChannelsAvailable"]  >> '=' > int_         [phoenix::bind(&DigitizerBlock::totalChannelsAvailableSet , ptr, qi::_1)] > eol_);
        globCfdFrac    = (lexeme["GlobalCfdFraction"]       >> '=' > float_       [phoenix::bind(&DigitizerBlock::globalCfdFractionSet      , ptr, qi::_1)] > eol_);
        paramFile      = (lexeme["PerChannelParameterFile"] >> '=' > quotedString [phoenix::bind(&DigitizerBlock::perChannelParameterFileSet, ptr, qi::_1)] > eol_);
        
		// define the start rule which holds the whole monstrosity and set the rule to skip blanks
		// if we skipped spaces we could not parse newlines as separators
		startRule = skip(blank) [digitizerBlockRule];
		digitizerBlockRule = lexeme["[DigitizerBlock]"] >> *eol
                               > ( 
                                   totalChanAvail ^ globCfdFrac ^ paramFile
                                 )
                               > lexeme["[EndBlock]"];
	}

private:
    //base rules for the file
    qi::rule<Iterator>                 startRule;
	qi::rule<Iterator, qi::blank_type> digitizerBlockRule;
	
	// special sub grammars
	Utility::QuotedString<Iterator> quotedString;
	
	// parameters
	qi::rule<Iterator, qi::blank_type> totalChanAvail, globCfdFrac, paramFile;

	// hold the pointer that we are going to bind to
	DigitizerBlock* ptr;
};

}

}

#endif  // ORCHID_SRC_INPUTLIB_PARSERS_DIGITIZERBLOCKGRAMMAR_H
