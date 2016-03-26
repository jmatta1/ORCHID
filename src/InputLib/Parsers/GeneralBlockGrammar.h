/***************************************************************************//**
********************************************************************************
**
** @file GeneralBlockGrammar.h
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
** @details holds the grammars necessary for parsing general blocks into
** the general block structure
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_INPUTLIB_PARSERS_GENERALBLOCKGRAMMAR_H
#define ORCHID_SRC_INPUTLIB_PARSERS_GENERALBLOCKGRAMMAR_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include <boost/phoenix/bind/bind_member_function.hpp>
// includes from ECHOS
#include"InputLib/Blocks/GeneralBlock.h"
#include"utility.h"

namespace InputParser
{

namespace Parsing
{

namespace qi = boost::spirit::qi;

// the main gammar
template <typename Iterator>
struct GeneralBlockGrammar : qi::grammar<Iterator>
{
	GeneralBlockGrammar(GeneralBlock* gb) : GeneralBlockGrammar::base_type(startRule), ptr(gb)
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
        runTitle            = (lexeme["RunTitle"]            >> '=' > quotedString [phoenix::bind(&DigitizerBlock::runTitleSet           , ptr, qi::_1)] > eol_);
        warnRate            = (lexeme["WarnRate"]            >> '=' > int_         [phoenix::bind(&DigitizerBlock::warnRateSet           , ptr, qi::_1)] > eol_);
        updateFrequency     = (lexeme["UpdateFrequency"]     >> '=' > int_         [phoenix::bind(&DigitizerBlock::updateFrequencySet    , ptr, qi::_1)] > eol_);
        baseOutputDirectory = (lexeme["BaseOutputDirectory"] >> '=' > quotedString [phoenix::bind(&DigitizerBlock::baseOutputDirectorySet, ptr, qi::_1)] > eol_);
        
		// define the start rule which holds the whole monstrosity and set the rule to skip blanks
		// if we skipped spaces we could not parse newlines as separators
		startRule = skip(blank) [generalBlockRule];
		generalBlockRule = lexeme["[GeneralBlock]"] >> *eol
                               > ( 
                                   runTitle        ^ warnRate            ^
                                   updateFrequency ^ baseOutputDirectory
                                 )
                               > lexeme["[EndBlock]"];
	}

private:
    //base rules for the file
    qi::rule<Iterator>                 startRule;
	qi::rule<Iterator, qi::blank_type> generalBlockRule;
	
	// special sub grammars
	Utility::QuotedString<Iterator> quotedString;
	
	// parameters
	qi::rule<Iterator, qi::blank_type> runTitle,        warnRate,
	                                   updateFrequency, baseOutputDirectory;

	// hold the pointer that we are going to bind to
	GeneralBlock* ptr;
};

}

}

#endif  // ORCHID_SRC_INPUTLIB_PARSERS_GENERALBLOCKGRAMMAR_H
