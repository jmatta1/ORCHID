/***************************************************************************//**
********************************************************************************
**
** @file onedspectrum.cpp
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
** @details Holds implementation for functions in the 1-D spectrum class which
** allows the reading and writing of 1 dimensional spectra in radware format
**
********************************************************************************
*******************************************************************************/
#include "onedspectrum.h"
// includes for C system headers
// includes for C++ system headers
#include<fstream>
#include<vector>
// includes from other libraries
// includes from ORCHID
#include"stringutil.h"
#include"byteutil.h"


namespace Utility
{

namespace RdwrSpe
{
using std::vector;
using std::fstream;
using std::ios_base;
using std::endl;

OneDSpectrum::OneDSpectrum(const OneDSpectrum &copy):
		specName(copy.specName), channels(copy.channels),
		histogram((copy.channels>0)?(new float[copy.channels]):NULL)
{
	for(int i=0; i<channels; ++i)
	{
		histogram[i] = copy.histogram[i];
	}
}

OneDSpectrum::~OneDSpectrum()
{
	delete[] histogram;
}

//Implementation of the spectrum resize function
void OneDSpectrum::resizeSpec(int size)
{
	//If we don't need to resize, do nothing
	if(size==channels){return;}

	//If we are here then resizing is needed
	float *temporary = new float[size];
	if(size>channels)
	{
		//copy the original histogram
		for(int i=0; i<channels; ++i)
		{temporary[i]=histogram[i];}
		//fill the rest of the new histogram with zeros
		for(int i=channels; i<size; ++i)
		{temporary[i]=0;}
	}
	else
	{
		//copy the lower end of original histogram
		for(int i=0; i<size; ++i)
		{temporary[i]=histogram[i];}
	}
	delete[]histogram;
	histogram = temporary;
	channels = size;
	return;
}

//Implementation of the read spectrum from radware file function
bool OneDSpectrum::readSpectrum(const string &fileName)
{
	//create the file and attempt to open it
	fstream input;
	input.open(fileName.c_str(),ios_base::in | ios_base::binary);
	if(!input.is_open())
	{
		return false;
	}

	//We succeeded in opening the file now read it
	char* rawData;
	rawData = new char[8];
	bool reverse=false;

	//read the first integer and test its endianness
	input.read(rawData,4);
	int head = *(reinterpret_cast<int*>(rawData));

	if(head!=24)
	{//the byte order is probably inverted change that and retest
		input.seekg(0);
		input.read(rawData,4);
		swapFourByte(reinterpret_cast<int32*>(rawData),1);
		head = *(reinterpret_cast<int*>(rawData));
		if(head!=24){return false;}
	}

	//retrieve the spectrum name
	input.read(rawData,8);
	specName=string(rawData,8);

	//retrieve the number of channels
	input.read(rawData,4);
	if(reverse){swapFourByte(reinterpret_cast<int32*>(rawData),1);}
	channels = *(reinterpret_cast<int*>(rawData));

	//retrieve the next value it should be 1 for a simple 1D spectrum
	int ySize;
	input.read(rawData,4);
	if(reverse){swapFourByte(reinterpret_cast<int32*>(rawData),1);}
	ySize = *(reinterpret_cast<int*>(rawData));

	//now seek ahead to the start of the spectrum data
	input.seekg(32);
	input.read(rawData,4);
	if(reverse){swapFourByte(reinterpret_cast<int32*>(rawData),1);}
	head = *(reinterpret_cast<int*>(rawData));

	//if something is wrong clean up and return false
	if(head!=(static_cast<int>(sizeof(float))*ySize*channels))
	{specName=""; channels=0; return false;}

	//allocate array and read it
	if(histogram!=NULL){delete[] histogram;}
	histogram = new float[channels];

	for(int i=0; i<channels; ++i)
	{
		input.read(rawData,4);
		histogram[i] = *(reinterpret_cast<float*>(rawData));
	}

	//clean up file and datastream
	if(reverse){swapFourByte(reinterpret_cast<int32*>(histogram),channels);}

	delete[] rawData;

	return true;
}

//Implementation of the read spectrum from csv file function
bool OneDSpectrum::readCSV(const string &fileName)
{
	//attempt to open the file, if we fail, exit
	fstream input;
	input.open(fileName.c_str(),ios_base::in);
	if(!input.is_open()){return false;}

	//the file is open, proceed
	string line;
	//get rid of the "channel, counts" line
	std::getline(input,line,'\n');
	vector<string> spec;

	while(!(getline(input,line,'\n').eof()))
	{
		spec.push_back(line.substr(line.find_first_of(',',0)+1));
	}

	channels = static_cast<int>(spec.size());
	specName = "";
	if(histogram!=NULL){delete[] histogram;}
	histogram = new float[channels];

	StringConv conv;
	float temp=0;

	for(int i=0; i<channels; ++i )
	{
		conv.fromString<float>(temp,spec[static_cast<long unsigned int>(i)]);
		histogram[i] = temp;
	}
	return true;
}

//Implementation of the read spectrum to radware file function
bool OneDSpectrum::writeSpectrum(const string &fileName)
{
	//attempt to open the file, if we fail exit
	fstream output;
	output.open(fileName.c_str(),ios_base::out | ios_base::trunc |  ios_base::binary);
	if(!output.is_open()){return false;}

	//the file is open, write to it
	char *rawData;
	int head=24;

	//write the header record size to the file
	rawData = reinterpret_cast<char*>(&head);
	output.write(rawData, 4);

	//trim the name and write it to the string
	string temp=specName.substr(0,7);
	output.write(temp.c_str(),static_cast<long int>(temp.size()+1));
	//just in case the string was less than 8 characters (counting the null terminator)
	rawData = new char;
	*rawData=0;
	for(unsigned i=0; i<(8-(temp.size()+1)); ++i)
	{	output.write(rawData,1);	}
	delete rawData;

	//Select whether to use 16384 of channels as the number of channels
	//(Whichever is less
	int numChan = (channels>16384)?16384:channels;

	//write the number of channels to the file
	rawData = reinterpret_cast<char*>(&numChan);
	output.write(rawData,4);

	//write the next few ints to the file
	head = 1;
	rawData = reinterpret_cast<char*>(&head);
	output.write(rawData, 4);
	output.write(rawData, 4);
	output.write(rawData, 4);

	//write the header record length ending
	head = 24;
	rawData = reinterpret_cast<char*>(&head);
	output.write(rawData, 4);

	//write the spectrum record length
	head = static_cast<int>(sizeof(float))*numChan;
	rawData = reinterpret_cast<char*>(&head);
	output.write(rawData, 4);

	//write the spectrum to the file
	for(int i=0; i<numChan; ++i)
	{
		rawData  = reinterpret_cast<char*>(histogram+i);
		output.write(rawData, 4);
	}

	//write the spectrum record length ending
	head = static_cast<int>(sizeof(float))*numChan;
	rawData = reinterpret_cast<char*>(&head);
	output.write(rawData, 4);

	return true;
}

//Implementation of the read spectrum to csv file function
bool OneDSpectrum::writeCSV(const string &fileName)
{
	//open the file
	fstream output;
	output.open(fileName.c_str(),ios_base::out | ios_base::trunc);
	if(!output.is_open()){return false;}

	output<<"Channel Number, Counts"<<endl;

	output.precision(8);

	for(int i=0; i<channels; ++i)
	{
		output<<i<<" , "<<histogram[i]<<endl;
	}


	return true;
}

}
}
