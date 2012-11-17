//DFTGeneric.cpp
#include "DFTGeneric.h"

using namespace std;
namespace DFT{
	//GetOffset()
	//Calculate the offset index based on the parameters
	unsigned int DFTGeneric::GetOffset(unsigned int intervalN, unsigned int dimension) const{
		return intervalN*Dimension + dimension;
	}
	//CreateInterval() - Recursive edition. No good
	/*
	void DFTGeneric::CreateInterval(unsigned int intervalN){
		//Check if the nth interval exist. If not, create it with all the appropriate dimensions
		unsigned offset = GetOffset(intervalN, 0);
		try{
			Data.at(offset);
		}
		catch(std::out_of_range){
			//OK it doesn't exist
			//Check if the previous interval exist and create accordingly
			if (intervalN != 0){
				CreateInterval(intervalN-1);
			}
			//Create
			for (unsigned i = 0; i < Dimension; i++){
				Data.push_back(complex<double>(0,0));
			}
		}
	}
	*/
	//CreateInterval() - Better edition
	void DFTGeneric::CreateInterval(unsigned int intervalN) const{
		//Check if the nth interval exist. If not, create it with all the appropriate dimensions
		unsigned offset = GetOffset(intervalN, Dimension-1);
		if (offset >= Data.size()){
			for (unsigned i = Data.size(); i <= offset; i++){
				Data.push_back(complex<double>(0,0));
			}
		}
	}

	//Get Sample
	std::complex<double> DFTGeneric::DFTGet(unsigned int intervalN, unsigned int dimension) const{
		//Make sure the interval exist
		CreateInterval(intervalN);
		return Data[GetOffset(intervalN, dimension)];
	}

	//Set sample
	void DFTGeneric::DFTSet(unsigned int intervalN, unsigned int dimension, const std::complex<double> &data){
		//Make sure the interval exist
		CreateInterval(intervalN);
		unsigned offset = GetOffset(intervalN, dimension);
		Data[offset] = data;
	}

	//Properties Changer
	//Change number of intervals
	void DFTGeneric::DFTSetNumInterval(unsigned int n){
		if (!n){
			throw Exception(EXCEPTION_DATA_INVALID, "Number cannot be zero!");
		}
		if (n == DFTNumInterval()){
			return;
		}
		Data.resize(n*Dimension);
	}
	//Change Number of Dimensions.
	void DFTGeneric::DFTSetDimension(unsigned int n){
		//This is a pain in the arse operation.
		if (!n){
			throw Exception(EXCEPTION_DATA_INVALID, "Number cannot be zero!");
		}
		if (n == Dimension){
			return;
		}
		Data.resize(DFTNumInterval()*n);
		Dimension = n;
	}

}