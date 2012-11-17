#pragma once
#ifndef WaveWord_H
#define WaveWord_H
#include "WaveMisc.h"
#include <string>
using namespace std;

namespace Wave{

	/*
		A "Word" is an array of characters WORD_SIZE in bytes.
		This is basically a wrapper for an array of characters with associated methods.
		This array is NOT a C String.

		This method throws range_error exceptions in the case of out of range access.
	*/
	class Word{
		char Data[WORD_SIZE];
		unsigned int Size;			//The number of bytes that are set. Not really reliable. 

	protected:
		unsigned int GetSum() const;		//For internal use only. Get the sum of all the bytes in the word. 
		
	public:
		//Constructor
		Word();
		Word(const char *data, unsigned int size = WORD_SIZE);	
		Word(const unsigned char* data , unsigned int size = WORD_SIZE);		//Unsigned version
		Word(string data);

		//Getter and Setter
		char GetByte(unsigned int i) const;					//Get the nth byte
		void SetByte(unsigned int i, char data);		//Set the nth byte. If i > Size, will change Size.

		unsigned int GetSize();							//Get number of bytes that are set
		void SetSize(unsigned n);						//Set number of bytes set.

		char *GetPointer();								//Get pointer to the chraracter array
		string GetString();								//Get the word as a string. First byte is first character of string. Does not bother with endianess.

		bool IsEmpty();									//Checks to see if word is empty. i.e. Size is zero.
		bool IsAllNull();								//Checks if all the bytes in the word is \0

		void PadBytes(Endianess endian=Little);			//Based on the endinaness of the data, pad the word to the maximum word size with zeroes.

		//Overloaded Operators
		char& operator[](unsigned int i);				//If i > Size, will change Size.
		char const& operator[](unsigned int i) const;	//See http://www.parashift.com/c++-faq-lite/const-correctness.html#faq-18.12

		//Overloaded Comparison Operators
		bool operator<(const Word &op) const;			//For use in std::map as the key

		bool operator==(const Word &op) const;			//Equality comparison. If your comparing operator contain null, use this.
		//bool operator==(const char *op) const;		//C String Overload. Note: If your comparing operator contains null characters, not recommended you use this.
		//bool operator==(const string &op) const;		//String Overload Note: If your comparing operator contains null characters, not recommended you use this.

		bool operator!=(const Word &op) const;	
	};

	/* Additional overloaded version of GetSignedInt and GetUnsigned Int to handle Word */
	//If word is longer than four bytes, the more significant bytes according to the Endianess will be truncated.
	unsigned int GetUnsignedInt(Word data, Endianess endian=Little);
	int GetSignedInt(Word data, Endianess endian=Little);

	/* Convert an integer, signed or unsigned, into a series of bytes according to Endian. */
	//Returns the number of bytes needed to fill the data
	Word GetBytesFromInt(int data, Endianess endian = Little);
	Word GetBytesFromUnsigned(unsigned int data, Endianess endian = Little);
}

#endif /* WaveWord_H */