/***
	Helper class to create a larger buffer for file output

	Automatic deallocation of memory helps to reduce risk of memory leaks

	Construct the object to allocate buffer. When the object goes out of scope or is no longer accessible 
	or is destroyed, deallocation takes place via the destructor

	There is no point calling the methods of this class actually

	bad_alloc might be thrown and will not be caught.
***/

#pragma once
#ifndef OutputBuffer_H
#define OutputBuffer_H

#include <fstream>
using namespace std;

class OutputBuffer{
	char *Buffer;			//Buffer
public:
	size_t Size;												//Size of buffer
	OutputBuffer(ofstream &file, size_t size=1048576);			//Constructor
	OutputBuffer(OutputBuffer &op);								//Copy Constructor
	~OutputBuffer();											//Destructor
	OutputBuffer operator=(const OutputBuffer &op);				//Assignment
protected:				
	void Destroy();												//Destroy
	void Assign(ofstream &file, size_t size);					//Assign
};

#endif /*OutputBuffer_H*/