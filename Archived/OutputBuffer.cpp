/***
	Helper class to create a larger buffer for file output

	Automatic deallocation of memory helps to reduce risk of memory leaks

	Construct the object to allocate buffer. When the object goes out of scope or is no longer accessible 
	or is destroyed, deallocation takes place via the destructor

	There is no point calling the methods of this class actually
***/
#include <new>
#include "OutputBuffer.h"
//Constructor
OutputBuffer::OutputBuffer(ofstream &file, size_t size){
	Buffer = NULL;
	Assign(file, size);
}
//Copy constructor
OutputBuffer::OutputBuffer(OutputBuffer &op){
	//... why will you do this?
	throw bad_alloc("Attempted to copy a buffer");
}
//Destructor
OutputBuffer::~OutputBuffer(){
	Destroy();
}
//Assignment operator
OutputBuffer OutputBuffer::operator=(const OutputBuffer &op){
	throw bad_alloc("Attempted to assign a buffer");
}
//Destroy
void OutputBuffer::Destroy(){
	if (Buffer != NULL){
		delete[] Buffer;
	}
}
//Assign
void OutputBuffer::Assign(ofstream &file, size_t size){
	Destroy();
	Size = size;
	//If it fails, a bad_alloc exception will be thrown and will not be caught. You should be able to recover and use 
	//the default buffer
	Buffer = new char[size];				
	file.rdbuf()->pubsetbuf(Buffer, size);
}