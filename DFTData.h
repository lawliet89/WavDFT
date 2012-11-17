/*
	DFTData

	This is an abstract base class that all the data that is to be passed to DFT
	must inherit from. It defines some required methods that the DFT classes expect
	to use to retrieve the data

	It's up to the class to determine HOW to set and store the data. 

	DFTTime 
	A derived class from DFTData that simply declares itself as a time domain class

	DFTFrequency
	A derived class from DFTData that simply declares itself as a frequency domain class
*/
#pragma once
#ifndef DFTData_H
#define DFTData_H

#include <complex>
#include "Exception.h"

namespace DFT{

	/********** DFTDATA *************/
	class DFTData{
	public:
		enum Domain { Time, Frequency };				//Type enum
		virtual ~DFTData(){}							//Virtual Destructor - cf http://www.parashift.com/c++-faq-lite/virtual-functions.html#faq-20.7
		virtual unsigned int DFTDimension() const = 0;		//Returns the number of dimensions per discrete sample
		virtual unsigned int DFTSample() const = 0;			//Returns the number of discrete samples
		virtual unsigned int DFTNumInterval() const = 0;		//Gets the number of intervals
		virtual Domain DFTDomain() const = 0;					//Return the domain the data is stored in
		//Returns Interval. If Time Domain, returns the time interval between samples. If Freq Domain, returns the frequency interval between values
		virtual double DFTInterval() const = 0;			

		//Optional Setters
		virtual void DFTSetDimension(unsigned int n){		//Set the number of dimensions. Can be unsupported.
			throw Exception(EXCEPTION_UNSUPPORTED, "Unsupported operation");
		}	
		virtual void DFTSetNumInterval(unsigned int n){			//Set the number of intervals. Can be unsupported
			throw Exception(EXCEPTION_UNSUPPORTED, "Unsupported operation");
		}
		virtual void DFTSetInterval(double n){				//Set the interval between samples. Can be unsupported
			throw Exception(EXCEPTION_UNSUPPORTED, "Unsupported operation");
		}

		//Get value of the sample at the particular interval at the particular dimension
		//Index starts from ZERO
		virtual std::complex<double> DFTGet(unsigned int interval, unsigned int dimension) const = 0;		

		//Set value of the sample at the particular interval at the particular dimension
		//Hint: If classes are worried about transforms modifying their original data,
		//they are free to store the original data and edited data separately.
		//But there should not be a reason to do so
		//Index starts from ZERO
		virtual void DFTSet(unsigned int intervalN, unsigned int dimension, const std::complex<double> &data) = 0;
	};

	/********** DFTTime *************/
	class DFTTime: public virtual DFTData{
	public:
		virtual ~DFTTime(){}
		Domain DFTDomain() const{ return Time; };
	};

	/********** DFTFrequency *************/
	class DFTFrequency: public virtual DFTData{
	public:
		virtual ~DFTFrequency(){}
		Domain DFTDomain() const{ return Frequency; };
	};
}

#endif /*DFTData_H*/