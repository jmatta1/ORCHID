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
                     > (lineRule1 > lineRule2)
                     > *(+eol_ >> (lineRule1 > lineRule2))
                     > *eol_
                     > eoi
                   );

        lineRule1 = ( int_         [phoenix::bind(&DigitizerChannelData::addModuleNumber,       ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addChannelNumber,      ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addChannelEnable,      ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addRecordLength,       ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addLargeRange,         ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addAggregateEvents,    ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addPreTrigger,         ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addCfdDelay,           ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addCfdFraction,        ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addShortGate,          ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addLongGate,           ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addGateOffset,         ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addTrigThreshold,      ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addFixedBaseline,      ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addShapedTrigWidth,    ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addTrigHoldOff,        ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addPsdThreshold,       ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addChargeSensitivity,  ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addChargePedestalOn,   ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addDppTriggerCounting, ptr, qi::_1)] > lexeme[',']);
    
        lineRule2 = ( int_         [phoenix::bind(&DigitizerChannelData::addDiscMode,              ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addPulsePolarity,         ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addTrigMode,              ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addBaselineMean,          ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addDisableSelfTrigger,    ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addPsdCutBelowThresh,     ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addPsdCutAboveThresh,     ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addOverRangeRejection,    ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addTriggerHysteresis,     ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addUseLocalShapedTrig,    ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addLocalShapedTrigMode,   ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addUseLocalTrigVal,       ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addLocalTrigValAsVeto,    ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addExtrasWordOptions,     ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addSmoothIntegration,     ptr, qi::_1)] > lexeme[','] >
                      boolSymbols_ [phoenix::bind(&DigitizerChannelData::addInputSmoothing,        ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addDcOffset,              ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addVetoDurationExtension, ptr, qi::_1)] > lexeme[','] >
                      int_         [phoenix::bind(&DigitizerChannelData::addTriggerValidMas,       ptr, qi::_1)] );
                  

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
    qi::rule<Iterator, qi::blank_type > lineRule1;
    qi::rule<Iterator, qi::blank_type > lineRule2;
    
    //data holder
    DigitizerChannelData* ptr;
};

}

}

#endif //ORCHID_SRC_INPUTLIB_PARSERS_DIGITIZERMODULEPARSER_H
