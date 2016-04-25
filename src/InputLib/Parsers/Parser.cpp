/***************************************************************************//**
********************************************************************************
**
** @file Parser.cpp
** @author James Till Matta
** @date 26 Mar, 2016
** @brief Implementation file for the function that calls the boost::spirit parser
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Implementation for the function that handles parsing the block
** input files whose grammars are constructed. This exists to offload the
** template instantiation to a file where things should not change during later
** development
**
********************************************************************************
*******************************************************************************/
#include"InputLib/Parsers/Parser.h"
// includes for C system headers
// includes for C++ system headers
#include<fstream>
// includes from other libraries
#include <boost/spirit/include/qi.hpp>
// includes from ORCHID
#include"FileParser.h"

namespace InputParser
{

typedef boost::spirit::istream_iterator It;

bool parseBlockInputFile(InputParameters* inParams, const std::string& inputFileName)
{
    //make the parser, this line will make the compiler *GRIND* as it has to
    //work its way through the huge amount template stuff
    Parsing::InputFileParser<It> inputFileParser(inParams);
    //set up the file
    std::ifstream input(inputFileName);
    //unset the skip whitespace
    input.unsetf(std::ios::skipws);
    //set up the iterators
    It start(input), stop;
    //parse the damn thing
    return boost::spirit::qi::parse(start, stop, inputFileParser);
}

}
