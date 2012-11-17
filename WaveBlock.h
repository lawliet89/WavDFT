/*
	[ADT]

	Data Container for one Block of N Channels sample in the Wave File.
	Should not be instantiated manually.

	If attempting to access data outside of the range, range_error is thrown
	During memory allocation, if the process fails, the exception thrown is not caught.
	It is up to the caller to catch the exception.

	If WaveBlock is cast to an integer, it returns the number of channels.

	The specification allows for up to 255 bits per sample! This is way to large and this implementation only
	supports up to 4 bytes aka 32 bits.

	Also, we are assuming that the sample size is a multiple of 8 bits.

	Note: Channel index starts from zero.

	Copy Constructor, Assignment Overload implemented.
*/
#pragma once
#ifndef WaveBlock_H
#define WaveBlock_H

#include <new>
using namespace std;

namespace Wave{
	//Inherit some class for DFT purposes?
	template <typename T=int> class WaveBlock{
		unsigned int ChannelsCount;		//Number of channels
		T *SampleData;		//Pointer to an array of sample data. Size is determined by number of channels.

	public:

		/*************************
		**	    Constructors	**
		**************************/
		WaveBlock(): ChannelsCount(0), SampleData(NULL){}
		
		//Construct the object with intiial data
		//Pass a pointer to an array with the data
		WaveBlock(unsigned int channels, T *data): SampleData(NULL){
			SetChannelsCount(channels);
			for (unsigned int i = 0; i < channels; i++){
				SetChannel(i, *(data+i));
			}
		}

		///Because we do manual memory allocation, we have to have a copy constructor
		WaveBlock(const WaveBlock &obj): SampleData(NULL){
			SetChannelsCount(obj.GetChannelsCount());
			for (unsigned int i = 0; i < obj.GetChannelsCount(); i++){
				SetChannel(i, obj.GetChannel(i));
			}
		}

		/*************************
		**	    Destructor		**
		**************************/
		~WaveBlock(){
			delete[] SampleData;		//Delete memory allocated to prevent memory leaks
		}
	
		//Set Number of channels. If you change the number of channels, any prior data is automatically lost.
		//You must CALL THIS METHOD FIRST before you even do any setting of data
		void SetChannelsCount(unsigned int n);		
		unsigned int GetChannelsCount() const;						//Get the number of channels

		T GetChannel(unsigned int n) const;		//Get the nth channel
		void SetChannel(unsigned int n, T value);		//Set the nth channel to value

		bool IsEmpty();					//See if the number of channels is zero AND if SampleData is a NULL pointer

		/*****************************
		**  Overloaded Operators	**
		******************************/
		T& operator[](unsigned int n);			//Use the object like an array
		operator int(){ return ChannelsCount; }
		WaveBlock operator=(const WaveBlock &op);			//Because we use dynamic memory allocation we better overload this!	
	};

	//SetChannelsCount()
	template <typename T> void WaveBlock<T>::SetChannelsCount(unsigned int n){
		delete[] SampleData;		//IF data already exists, we must destroy them to prevent any memory leaks
		ChannelsCount = n;
		SampleData = new T[n];
	}

	//Simple "Getter" and "Setters
	//GetChannelsCount()
	template <typename T> unsigned int WaveBlock<T>::GetChannelsCount() const{
		return ChannelsCount;
	}

	//GetChannel()
	template <typename T> T WaveBlock<T>::GetChannel(unsigned int n) const{
		if (n >= ChannelsCount){		//Range checking
			throw Exception(EXCEPTION_RANGE,"Out of range access.");
		}
		return *(SampleData+n);
	}

	//SetChannel()
	template <typename T> void WaveBlock<T>::SetChannel(unsigned int n, T value){
		if (n >= ChannelsCount){		//Range checking
			throw Exception(EXCEPTION_RANGE,"Out of range access.");
		}
		*(SampleData+n) = value;
	}
	//IsEmpty()
	template <typename T> bool WaveBlock<T>::IsEmpty(){
		return (ChannelsCount == 0 && SampleData == NULL);
	}
	//Overloaded Operators
	//Operator []
	template <typename T> T& WaveBlock<T>::operator[](unsigned int n){
		if (n >= ChannelsCount){		//Range checking
			throw Exception(EXCEPTION_RANGE,"Out of range access.");
		}
		return *(SampleData+n);
	}
	//Operator = (assignment)
	template <typename T> WaveBlock<T> WaveBlock<T>::operator=(const WaveBlock &op){
		//Handle self-assignment: http://www.parashift.com/c++-faq-lite/assignment-operators.html#faq-12.3
		//Otherwise, we will delete ourselves!
		if (&op == this) return *this;

		SetChannelsCount(op.GetChannelsCount());
		for (unsigned int i = 0; i < op.GetChannelsCount(); i++){
			SetChannel(i, op.GetChannel(i));
		}
		return *this;
	}
}
#endif /* WaveBlock_H */