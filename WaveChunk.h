#pragma once
#ifndef WaveChunk_H
#define WaveChunk_H

#include <new>
#include "WaveMisc.h"
#include "WaveWord.h"

namespace Wave{
	/*
		A WaveChunk is a simple wrapper for a "Subchunk" of the Wave file.
		You should not manually instantiate the class.

		See http://www.lightlink.com/tjweber/StripWav/WAVE.html#Intro
	*/
	template <typename T=vector<Word> > class WaveChunk{
		T Data;				//Data for the wave chunk. Handling of Wave chunk data is left up to the program
		Word ID;			//The ID of the chunk
		unsigned int Size;	//Size in bytes of the chunk
		bool Complete;	//Set to false if the chunk is incomplete.

	public:
		//Constructors
		WaveChunk(): Size(0), Complete(false) { }
		WaveChunk(const Word &id, unsigned int size, const T &data, bool complete)
					: ID(id), Size(size), Data(data), Complete(complete)
					{}

		//Setters
		void Set(const Word &id);				//Set ID
		void Set(unsigned int size);		//Set size
		void Set(const T &data);				//Set Data
		void Set(bool complete);			//Set to complete

		//Getters
		T &GetData(){ return Data; }					//Get data
		T GetData() const { return Data; }
		Word GetID() const{ return ID; }				//Get the ID of the chunk
		unsigned int GetSize() const{ return Size; }		//Get the Data of the chunk
		bool GetComplete() const{ return Complete; }
	};
	
	//Setters
	template <typename T> void WaveChunk<T>::Set(const Word &id){
		ID = id;
	}
	template <typename T> void WaveChunk<T>::Set(unsigned int size){
		Size = size;
	}
	template <typename T> void WaveChunk<T>::Set(const T &data){
		Data = data;		//Make a bitwise copy of the data. Or use the overloaded copy operator.
	}
	template <typename T> void WaveChunk<T>::Set(bool complete){
		Complete = complete;
	}
}

#endif