/*
	DFTGeneric is a Generic implementation of DFT Data.
	DFTGeneric is STILL an abstract base class

	DFTGenericTime is a generic time domain implementation
	DFTGenericFrequency is a generic Frequency domain implementation

	These classes are in a diamond shaped inheritance. 
	See http://www.parashift.com/c++-faq-lite/multiple-inheritance.html
	for more details on the specifics of the care needed for their implementation
*/
#pragma once
#ifndef DFTGeneric_H
#define DFTGeneric_H

//Suppress "Dreaded Diamond: Warning cf http://msdn.microsoft.com/en-us/library/6b3sy7ae(v=VS.100).aspx
#pragma warning( disable : 4250 )

#include "DFTData.h"
#include "Exception.h"
#include <vector>

namespace DFT{

	/************** DFTGeneric ******************/
	class DFTGeneric: public virtual DFTData{
		unsigned int Dimension;				//The number of dimensions
		double Interval;					//Interval between samples
		mutable std::vector<complex<double> > Data;		//The data

	protected: //Protected internal methods
		unsigned int GetOffset(unsigned int intervalN, unsigned int dimension) const;		//Get the offset based on the param
		void CreateInterval(unsigned int intervalN) const;									//Create the interval.

	public:
		//Construct the object.
		//n is the number of dimensions, interval is the interval.
		//Size is the projected number of samples. This is so that memory for the data structure can be allocated accordingly.
		//If not set, will not do any allocation
		DFTGeneric(unsigned int n=1, double interval = 1.0, unsigned int size=0): Dimension(n), Interval(interval) {
			if (Dimension == 0 || Interval <= 0){
				throw Exception(EXCEPTION_DATA_INVALID, "Dimension and/or interval cannot <= zero!");
			}
			if (size){
				Data.reserve(size);
				Data.clear();		//Irritating problem with reserve creating element zero
			}
		}
		//Virtual destructor
		virtual ~DFTGeneric() { }

		//Properties Getter
		unsigned int DFTDimension() const{ return Dimension; }				//Return the number of dimensions
		double DFTInterval() const{ return Interval; }						//Returns the interval
		unsigned int DFTSample() const{ return Data.size(); }					//Returns number of discrete samples
		unsigned int DFTNumInterval() const{ return Data.size()/Dimension; }	//Returns number of intervals

		//Properties Setter
		void DFTSetInterval(double n){								//Set interval
			if (n <= 0){
				throw Exception(EXCEPTION_DATA_INVALID, "Interval cannot <= zero!");
			}
			Interval = n;
		}
		void DFTSetDimension(unsigned int n);			//Set Number of dimensions - note this operation will render existing data invalid
		void DFTSetNumInterval(unsigned int n);			//Set number of intervals

		//Samples getter and setter
		std::complex<double> DFTGet(unsigned int intervalN, unsigned int dimension) const;					//Get sample
		void DFTSet(unsigned int intervalN, unsigned int dimension, const std::complex<double> &data);		//Set sample

	};

	/************** DFTGenericTime *************/
	class DFTGenericTime: public DFTGeneric, public DFTTime{ 
	public:
		//Constructor
		DFTGenericTime(unsigned int n=1, double interval = 1, unsigned int size=0): DFTGeneric(n, interval, size) { }
	};
	/************** DFTGenericFrequency ********/
	class DFTGenericFrequency: public DFTGeneric, public DFTFrequency{ 
	public:
		//Constructor
		DFTGenericFrequency(unsigned int n=1, double interval = 1, unsigned int size=0): DFTGeneric(n,interval, size) { }
	};
}

#endif /*DFTGeneric_H*/