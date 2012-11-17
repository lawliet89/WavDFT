/*
	Misc DFT Utility Functions
*/
#pragma once
#ifndef DFTUtility_H
#define DFTUtility_H

#include "DFTData.h"

namespace DFT{
	/* File Functions */
	//Write the values of the samples into a CSV file. Beware of exceptions thrown!
	//Data is a pointer to the data object, file is a C string of the file name 
	//Set buffer to a non-zero size to allow for a larger buffer rather than the default buffer
	void DumpFile(const DFTData *data, const char *file, unsigned int buffer=0);
}

#endif /*DFTUtility_H*/