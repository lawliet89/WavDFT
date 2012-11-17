/*
	Miscellaneous Wave Namespace Definitions
*/
#pragma once
#ifndef WaveMisc_H
#define WaveMisc_H

#include <string>
using namespace std;

namespace Wave{
	//Constants Definition
	const unsigned int WORD_SIZE = 4U;				//Definition of the number of Bytes/Chars in a Word
	const unsigned int MAX_SAMPLE_SIZE = 32U;		//Max sample size in bits. This MUST be equal to WORD_SIZE.

	//Wave Format definition
	const unsigned short WAVE_FORMAT_PCM = 1U;
	const unsigned short WAVE_FORMAT_EXTENSIBLE = 0xFFFE;

	//Wave Specific Exception Error Codes
	const int WAVE_RIFF_MISSING = 0x9001;			//RIFF header missing
	const int WAVE_NOT_WAVE = 0x9002;				//Not a Wave file
	const int WAVE_FMT_MISSING = 0x9003;			//'fmt ' subchunk is missing
	const int WAVE_FMT_INCOMPLETE = 0x9004;			//'fmt ' subchunk is missing information required.
	const int WAVE_NOT_PCM = 0x9005;				//File is not encoded in PCM
	const int WAVE_BITRATE_HIGH = 0x9006;			//Bitrate is too high
	const int WAVE_DATA_MISSING = 0x9007;			//Missing data even though it's supposed to be there

	/*
		Internally used constants
	*/
	//EOF of file encountered
	//const unsigned int EXCEPTION_EOF = 0x0000000f;					
	//The Word read was not full (i.e. not WORD_SIZE bytes) and in Big-Endian style, the bytes not read were set to null
	//const unsigned int EXCEPTION_WORD_NOT_FULL = 0x000000f0;			
	
	/*
		Data Constructs
	*/
	enum Endianess { Little, Big };

	/*
		Utility Functions
	*/
	/*
		Endianess: Little Endian - LSB Smaller Byte Address
					Big Endian - LSB Largest Byte Address (akin to how we read)
	*/
	//From the string data, reconstruct the signed integer that forms  it. Data is in endian format.
	//If data contains more than four bytes, the MSB will be truncated.
	//The first bit of the MSB is taken as the sign in the two's complement.
	//Length is in bytes
	int GetSignedInt(string data, Endianess endian=Little);			//String overloaded version for... "convenience sakes"
	int GetSignedInt(const char *data, unsigned int length=WORD_SIZE, Endianess endian=Little);

	//From the string data, reconstruct the unsigned integer that froms it. Data is in endian format.
	//If data contains more than four bytes, the MSB will be truncated.
	//Length is in bytes
	unsigned int GetUnsignedInt(string data, Endianess endian=Little);	//String overloaded version for... "convenience sakes"
	unsigned int GetUnsignedInt(const char *data, unsigned int length=WORD_SIZE, Endianess endian=Little);

}
#endif /* WaveMisc_H */