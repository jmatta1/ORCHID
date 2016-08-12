/***************************************************************************//**
********************************************************************************
**
** @file DigitizerModuleParser.h
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
#ifndef ORCHID_SRC_INPUTLIB_PARSERS_DIGITIZERMODULEPARSER_H
#define ORCHID_SRC_INPUTLIB_PARSERS_DIGITIZERMODULEPARSER_H


// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
// includes from ORCHID
#include"InputLib/Blocks/DigitizerModuleData.h"
#include"Utility.h"

namespace InputParser 
{

namespace Parsing
{

namespace qi = boost::spirit::qi;

template <typename Iterator>
struct DigitizerModuleParser : qi::grammar<Iterator>
{
    DigitizerModuleParser(DigitizerModuleData* data) :
        DigitizerModuleParser::base_type(start),
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
        lineRule = ( linkType_    [phoenix::bind(&DigitizerModuleData::addLinkType,                   ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addLinkNumber,                 ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addDaisyChainNumber,           ptr, qi::_1)] > lexeme[','] >
	  lexeme["0x"] > hex          [phoenix::bind(&DigitizerModuleData::addVmeBaseAddr,                ptr, qi::_1)] > lexeme[','] >
                     boolSymbols_ [phoenix::bind(&DigitizerModuleData::addEnableAutoFlush,            ptr, qi::_1)] > lexeme[','] >
                     boolSymbols_ [phoenix::bind(&DigitizerModuleData::addPropogateTrigs,             ptr, qi::_1)] > lexeme[','] >
                     boolSymbols_ [phoenix::bind(&DigitizerModuleData::addRecordWaveforms,            ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addAnalogProbe,                ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addDualTrace,                  ptr, qi::_1)] > lexeme[','] >
                     boolSymbols_ [phoenix::bind(&DigitizerModuleData::addRecExtrasWord,              ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addDigVirtProbe1,              ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addDigVirtProbe2,              ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addChanBuffPerAgg,             ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addTriggerCountingMethod,      ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addMemFullMode,                ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addPllRefClock,                ptr, qi::_1)] > lexeme[','] >
	  lexeme["0x"] > hex          [phoenix::bind(&DigitizerModuleData::addGlobalChanPairTrigMask,     ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addGlobalCoincidenceWindow,    ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addGlobalMajorityLevel,        ptr, qi::_1)] > lexeme[','] >
                     boolSymbols_ [phoenix::bind(&DigitizerModuleData::addExternalTrigger,            ptr, qi::_1)] > lexeme[','] >
	  lexeme["0x"] > hex          [phoenix::bind(&DigitizerModuleData::addChanPairTrigOutMask,        ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addTrigOutGenerationLogic,     ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addTrigOutMajorityLevel,       ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addExtTrigInTrigOut,           ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addMemBuffAlmostFullLevel,     ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addRunStartStopDelay,          ptr, qi::_1)] > lexeme[','] >
                     boolSymbols_ [phoenix::bind(&DigitizerModuleData::addUseExtTrigger,              ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addInterruptEventCount,        ptr, qi::_1)] > lexeme[','] >
                     int_         [phoenix::bind(&DigitizerModuleData::addAggregatesPerBlockTransfer, ptr, qi::_1)] );
                  

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
    DigitizerModuleData* ptr;
private:
    // symbol table to allow LinkType enumerations to be read in easily
    // also define a copy of it
    struct LinkType_ : qi::symbols<char, LinkType>
    {
        LinkType_(){
            this->add("Optical", LinkType::Optical)
                     ("Direct",  LinkType::DirectOptical)
                     ("USB",     LinkType::USB);
        }
    } linkType_;
};

}

}

#endif //ORCHID_SRC_INPUTLIB_PARSERS_DIGITIZERMODULEPARSER_H
