/*
	Defines the class container for the "Wave" file

	You can choose to #include this file only to include all necessary header files.

	Contains the metadata and the WavData object for the data of the object
	Class can only handle WAV files that are not compressed i.e. PCM

	In the case of errors, throws exceptions

	For documentation of WAVE File formats, refer to:
		- https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
		- http://www.lightlink.com/tjweber/StripWav/WAVE.html#Intro
		- http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
		- http://www.sonicspot.com/guide/wavefiles.html
*/
#pragma once
#ifndef WaveFile_H
#define WaveFile_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <complex>
using namespace std;
#include "WaveMisc.h"
#include "WaveBlock.h"
#include "WaveWord.h"
#include "WaveChunk.h"
#include "DFTData.h"

namespace Wave{
	/******************
		It is possible to instantiate this class with or without a file.

		However, if instantiating without a file, make sure to fill the neccessary data
		in the Data vector first before calling any methods.

		Most methods will check if there is any data in the vector and then call for a load data if there is none.
		Obviously, if there is no file, an exception will be thrown. Be aware.
	*******************/

	class WaveFile: public DFT::DFTTime{
		//Private Members & Methods
		map<Word, WaveChunk<> > SubChunks;		//Map to all the SubChunks except for the "data" SubChunk
		unsigned int ChunkSize;			//ChunkSize in bytes. Basically equal to File Size minus eight bytes.
		fstream *File;			//File Object for the Wave File. For input and output purposes.	
		
	protected:
		/*************************
			Protected Internal Methods

			These are generally utility methods.
		**************************/
		struct DataSubChunk_T{	//Information regarding the DataSubChunk. Only supports PCM. If non PCM detected, throws an exception during parsing	
			//Internal use
			streampos Begin;			//Value obtained from fstream::tellg to be used with fstream::seekg. The first byte of the data stream
			streampos End;				//The end streampos. This is the first byte AFTER the end of the data stream. So it might be past the last byte of file!
			bool IsLastChunk;			//Whether the data chunk is the last chunk of the whole file
			bool IsExtended;			//Set if Wave file is WAVE_FORMAT_EXTENSIBLE	
			bool IsLoaded;				//See if data is loaded or not
			vector<char>::iterator Iterator;			//Internal iterator for use during the case of reading from memory

			//Can be retrieved by public
			unsigned short FormatCode;	//Format Code
			unsigned int Size;			//Size in bytes
			unsigned int NumChannels;		//Number of Channels
			unsigned int SampleRate;		//Sample Rate - No of blocks per second
			unsigned int ByteRate;		//Bytes per second = SampleRate * NumChannels * BitsPerSample/8
			unsigned short BlockSize;	//Bytes per block = NumChannels * BitsPerSample/8
			unsigned short SampleSize;	//Bits per sample
			vector<char> Data;		//The data proper to be loaded into memory. Loaded by byte

			//"Construct data. Sets everything to zero
			DataSubChunk_T():
				Begin(0), End(0), IsLastChunk(false), IsExtended(false), IsLoaded(false),
				FormatCode(0), Size(0), NumChannels(0), SampleRate(0), ByteRate(0),
				BlockSize(0), SampleSize(0)
				{}
		} DataSubChunk;

		//These methods read without without bothering about the "endian-ness" of the content.
		//Usually, in this manner of file reading, if an integer value is spread over several bytes, the bytes will be arranged in Big-Endian
		//Then within each the byte, the interger will be in a LSB 0 Manner.
		//c.f. http://en.wikipedia.org/wiki/Endianess & http://en.wikipedia.org/wiki/Bit_numbering
				
		Word GetNextWord();				//Read the next word (with size defined by WORD_SIZE) 							
		char GetNextByte();				//Read the next byte in the file and returns a char. Throws exceptions on errors.
		//Get the next N number of bytes. If EOF is reached before n bytes are read, the list will be short. 
		//Check list length for confirmation or use File.eof() directly.
		vector<char> GetBytes(unsigned int n);	

	public:	
		/*************************
		**	    Constructor		**
		**************************/
		WaveFile():File(new fstream){
			//Does nothing. Creates an empty file.
		}
		WaveFile(char *file):File(new fstream){
			Open(file);	
		};

		//Copy Constructor
		WaveFile(const WaveFile &obj){
			//Copy data
			DataSubChunk = obj.DataSubChunk;
			SubChunks = obj.SubChunks;		
			ChunkSize = obj.ChunkSize;	
			File = new fstream;
		}
		//Assignment Operator
		WaveFile &operator=(const WaveFile &op){
			if (this == &op) return *this;			//Handle self assignment cf http://www.parashift.com/c++-faq-lite/assignment-operators.html#faq-12.3
			DataSubChunk = op.DataSubChunk;
			SubChunks = op.SubChunks;		
			ChunkSize = op.ChunkSize;	
			File = new fstream;
			return *this;
		}

		//Destructor
		~WaveFile();

		/*************************
		**	   Access Methods	**
		**************************/
		//Open the file for reading and writing
		void Open(const char *file);
		bool IsOpen(){ return File->is_open(); }				//Check if a file is open
		void Close(){ File->close(); }						//Close file

		//Parses the file and populate the SubChunks. If data already exist, they will be destroyed.
		//Return the number of subchunks discovered.
		//Does not load the DataSubChunk into memory
		unsigned int Parse();					

		//Methods that return Data Chunk properties
		unsigned int DataChunkSize() const { return DataSubChunk.Size; }			//Get size of data chunk
		unsigned int NumChannels() const {	return DataSubChunk.NumChannels; }		//Get number of channels
		unsigned int SampleRate() const{ return DataSubChunk.SampleRate; }		//Get Sample rate
		unsigned int ByteRate() const{ return DataSubChunk.ByteRate; }			//Bytes per second
		unsigned short BlockSize() const{ return DataSubChunk.BlockSize; }		//Get Block Size
		unsigned short SampleSize() const{ return DataSubChunk.SampleSize; }		//Get bits per sample
		unsigned int NumSamples() const;											//Calculate the number of samples. NB: Each channel counts as one sample
		unsigned int NumBlocks() const;											//Calculate the number of blocks.
		double Interval() const{ return 1/double(DataSubChunk.SampleRate); }				//Return number of intervals 

		//Edit the fmt chunk of this object. Call this method with a chunk created by CreatFmtChunk. Use parse() to update parsed data
		//void EditFmtChunk(const WaveChunk<> &fmt);			
		
		/****************************
		//Methods to relate to loading data
		*****************************/
		bool DataIsLoaded();							//Check if data is loaded and sets the internal flags accordingly
		
		//Load data into memory. BEWARE OF MEMORY CONSTRAINTS AND CATCH THE exceptions thrown by the vector class accordingly!
		void DataLoad();							
		void DataUnload();		//Unload

		//Dangerous methods - no need to put them under protected since... the original data is protected
		//vector<char> &DataGet(){ return DataSubChunk.Data; }		//Get a reference to the direct data for manipulation purposes
		//vector<char> DataGet() const{ return DataSubChunk.Data;}	//Get a copy of the data

		/****************************
		** "Iterator" Methods for reading Stream Data
		** If data was already loaded into memory, we will "read" from there instead. So much faster!
		*****************************/
		void DataRewind();						//Set the file pointer to point to the start of the data sub chunk
		WaveBlock<int> DataNextBlock();		//Get the next block of data as signed data
		WaveBlock<unsigned int> DataNextBlockUnsigned();	//Get the next block of data as unsigned data (use for Bitrate < 8)
		bool DataEnd();							//Check if end of Data has been reached. If file pointer is not within the  data chunk range, will also return true.

		/*********************
			Get and edit Audio Data
			 - With associated methods and overloaded operators

			 NOTE: Conversion between signed and unsigned do not change the bit pattern. So only one version,
			 that is the signed version, of the methods are provided for the getters
		*********************/
		int operator()(unsigned int interval, unsigned int dimension);	//Get sample at a specific interval and specific dimension
		int DataGet(unsigned int interval, unsigned int dimension);		//Alias for above  

		void DataEdit(unsigned int interval, unsigned int dimension, int data);			//Edit a specific sample
		void DataEdit(unsigned int interval, unsigned int dimension, unsigned data);		//Edit a specific sample

		char &operator[](unsigned int n);								//Get the nth byte from the data chunk
		char operator[](unsigned int n) const;							//Get the nth byte from the data chunk
		char DataGetByte(unsigned int n) const;								//Alias

		//Replace the entire data sub chunk. Call this with the vector created by CreateDataChunk
		//Use with care. Does not change the fmt subchunk.
		//void EditDataChunk(const WaveChunk<vector<char> > &data);

		/********************
			File operators
			 - These methods write from what is parsed and in memory
		***********************/
		void WriteFile();					//Based on everything contained in this object, write to the same file that was opened
		void WriteFile(const char *filename);	//Based on everything contained in this object, write to file name indicated
		void WriteFile(fstream &file);		//Based on everything contained in this object, write to file stream indicated
		
		/*********************
			Methods inherited from DFT::DFTData

			Don't call these methods. They are to be called by the DFT classes
		**********************/
		unsigned int DFTDimension() const{ return NumChannels(); } 		//Returns the number of dimensions per discrete sample
		unsigned int DFTSample() const{ return NumSamples(); }			//Returns the number of discrete samples
		double DFTInterval() const{ return Interval(); }		//The time interval between samples
		unsigned int DFTNumInterval() const{ return NumBlocks(); }					//Number of intervals

		//Alias for () operator
		//Our sound signal is, obviously, always real. 
		complex<double> DFTGet(unsigned int interval, unsigned int dimension) const;

		void DFTSet(unsigned int intervalN, unsigned int dimension, const std::complex<double> &data);

		/**********************
			Static Methods
		***********************/
		//Based on the data provided, construct a WaveFile Object
		static WaveFile CreateObject(unsigned _channels, unsigned _sampleRate, unsigned _sampleSize, const vector<char> &data);
		//Create a format chunk based on parameters - SAMPLE SIZE IS IN BITS
		static WaveChunk<> CreateFmtChunk(unsigned channels, unsigned sampleRate, unsigned sampleSize);
		//Create data chunk
		static WaveChunk<vector<char> > CreateDataChunk(const vector<char> &data);	
	};
}
#endif /* WaveFile_H */