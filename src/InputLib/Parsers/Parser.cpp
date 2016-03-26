/***************************************************************************//**
********************************************************************************
**
** @file blockinputfileparser.cpp
** @author James Till Matta
** @date 26 Mar, 2016
** @brief Implementation file for the class BlockInputFileParser
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition and implementation for the class that handles parsing
** the input files whose grammars are constructed
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
    //make the parser, this line will make the compiler *CHUG* as it has to
    //grind its way through the huge amount template stuff
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
