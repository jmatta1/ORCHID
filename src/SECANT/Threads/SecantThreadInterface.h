/***************************************************************************//**
********************************************************************************
**
** @file SecantThreadInterface.h
** @author James Till Matta
** @date 16 Dec, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Definition file for the abstract interface to SECANT threads this is
** used to hide the details of the derived templated threads from the type system
** so that I do not need to go really crazy in templatizing the SECANT core
**
********************************************************************************
*******************************************************************************/

namespace Secant
{

namespace Threads
{

class SecantThreadInterface
{
public:
    virtual void operator()() = 0;
};

}

}
