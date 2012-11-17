/*
	The DFT Abstract Base Class to allow for different DFT algorithmns to be implemented
	You should NEVER instantiate this class

	Explanation of operation:
	 - Each DFT object will take two different pointers:
		1) Pointer to a DFTTime object to store and retrieve time domain data
		2) Pointer to a DFTFrequency Object to store and retrieve frequency domain data
	 - Use the appropriate transform methods to retrieve data from the appropriate object and then store the result in the
	 other object

	 NOTE: It is the responsibility of the user to ensure the data objects has sufficient capacity to handle the 
	 number of data. i.e. the time domain object can handle as many samples as the freq domain object and vice versa
	 The generic classes provided has auto expansion mechanisms

*/
#pragma once
#ifndef DFT_H
#define DFT_H

#include "DFTData.h"

namespace DFT{
	class DFT{
	protected:
		DFTTime *TimeDomain;				//Time Domain Data
		DFTFrequency *FrequencyDomain;		//Frequency Domain Data
	public:
		//Constructor
		DFT(DFTTime *time = 0, DFTFrequency *freq = 0) : TimeDomain(time), FrequencyDomain(freq){}
		//Virtual Destructor
		virtual ~DFT(){}

		//************* Methods ********************//
		virtual void DiscreteFourierTransform() = 0;		//Perform Discrete Fourier Transform
		virtual void InverseDiscreteFourierTransform() = 0;	//Perform Inverse Discrete Fourier Transform

		//Getters - Do dynamic casts to the appropriate type actually implemented
		virtual DFTTime *GetTimeDomain(){ return TimeDomain; }					//Get a pointer to an internal object with the time domain data
		virtual DFTFrequency *GetFrequencyDomain(){ return FrequencyDomain;	}	//Get a pointer to an internal object with the frequency domain data

		//Setters
		//Set the pointers to the data objects
		virtual void SetPointer(DFTTime *ptr){ TimeDomain = ptr; }
		virtual void SetPointer(DFTFrequency *ptr){ FrequencyDomain = ptr; }
	};
}

#endif /* DFT_H */