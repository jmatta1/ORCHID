/***************************************************************************//**
********************************************************************************
**
** @file onedspectrum.h
** @author James Till Matta
** @date 27 Apr, 2016
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds definitions for functions in the 1-D spectrum class which
** allows the reading and writing of 1 dimensional spectra in radware format
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHID_SRC_UTILITY_SPE_ONEDSPECTRUM_H
#define ORCHID_SRC_UTILITY_SPE_ONEDSPECTRUM_H
// includes for C system headers
// includes for C++ system headers
#include <string>
// includes from other libraries
// includes from ORCHID
#include "floatutil.h"


namespace Utility
{

namespace RdwrSpe
{
using std::string;

class OneDSpectrum
{
public:
/*
**Construction and Destruction
*/
	//Produces a spectrum with a set number of channels and the given name (which will be truncated
	//to the appropriate 7 character length on writing to a file) defaults to an empty spectrum with
	//an empty name
	inline OneDSpectrum(int numChannels=0, const string &name="");
	//Copy constructor
	inline OneDSpectrum(const OneDSpectrum &copy);
	//destructor
	virtual ~OneDSpectrum();

/*
**File I/O Functions
*/
	//Reads a spectrum in the radware format from a given filename, returns true if the read
	//succeeded, returns false if the read failed
	bool readSpectrum(const string &fileName);

	//Reads a spectrum in csv format from a given filename returns true if the read succeeded
	//returns false otherwise
	bool readCSV(const string &fileName);

	//Writes a spectrum in the radware format from a given filename, returns false if the write
	//failed and true if it succeded
	bool writeSpectrum(const string &fileName);

	//Writes a spectrum in csv format from a given filename returns true if the write succeeded
	//returns false otherwise
	bool writeCSV(const string &fileName);

/*
**Mutator Functions
*/
	//Resizes the spectrum. If shrinking, the spectrum is shrunk from the upper end down
	//If expanding the extra channels are filled with 0s
	void resizeSpec(int size);

	//Sets the spectrum name to the given name if there are more than 7 characters present
	//the excess characters are cut off when the spectrum is saved to the file
	inline void setName(const string &rename);

	//adds one to the channel given, if the bin given is out of bounds it does nothing
	inline void incrChan(int channel);

	//adds the given value to the channel given, if the channel given is invalid nothing happens
	inline void addChan(int channel, float value);

	//sets the channel to the given value
	inline void setChan(int channel, float value);

/*
**Inspector Functions
*/
	//Returns the total number of channels
	inline int getSize();

	//Returns the spectrum name
	inline string getName();

	//Returns the number of counts in the channel number given. If the channel given is
	//out of bounds the function returns negative zero
	inline float getChan(int channel);


private:
	string specName;
	int channels;
	float* histogram;
};

inline OneDSpectrum::OneDSpectrum(int numChannels, const string &name):
		specName(name), channels(numChannels),histogram((numChannels>0)?(new float[numChannels]):NULL)
{}

void OneDSpectrum::setName(const string &rename)
{
	specName=rename;
}

void OneDSpectrum::incrChan(int channel)
{
	if(0<=channel && channel<channels)
	{
		histogram[channel]+=1;
	}
}


void OneDSpectrum::addChan(int channel, float value)
{
	if(0<=channel && channel<channels)
	{
		histogram[channel]+=value;
	}
}

int OneDSpectrum::getSize()
{
	return channels;
}

string OneDSpectrum::getName()
{
	return specName;
}

float OneDSpectrum::getChan(int channel)
{
	return ((channel<channels)?histogram[channel]:negZero());
}

void OneDSpectrum::setChan(int channel, float value)
{
	histogram[channel]=value;
}


}
}
#endif // ORCHID_SRC_UTILITY_SPE_ONEDSPECTRUM_H
