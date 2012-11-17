/*
	Class to handle the DFT using Matlab.

	Matlab uses FFTW to implement FFT.
*/
#pragma once
#ifndef DFTMatlab_H
#define DFTMatlab_H

#include "DFT.h"
#include "Exception.h"

//MATLAB Related
#include "engine.h"

namespace DFT{
	class DFTMatlab: public DFT{
		Engine *Matlab;						//MatLab Engine
		bool MatlabSet;

	public:
		//Constructor
		//Construct with pointers to the time domain and frequency domain objects
		//Use dynamic casts where appropriate
		DFTMatlab(DFTTime *time = 0, DFTFrequency *freq = 0, Engine *matlab=0) : DFT(time, freq){
			if (matlab == NULL){
				Matlab = engOpen("\0");
				MatlabSet = false;
			}
			else{
				Matlab = matlab;
				MatlabSet = true;
			}
			if (!Matlab){
				throw Exception(EXCEPTION_INITIALISATION, "Cannot initialise Matlab");
			}
			//Hides the engine
			engSetVisible(Matlab, false);
		}
		//Destructor
		~DFTMatlab(){
			if (!MatlabSet){
				engClose(Matlab);
			}
		}

		//Transform methods
		void DiscreteFourierTransform();		//Perform Discrete Fourier Transform
		void InverseDiscreteFourierTransform();	//Perform Inverse Discrete Fourier Transform

		//******** MATLAB Specific Methods **************//
		Engine *GetEngine(){ return Matlab; }
	};
}

#endif /*DFTMatlab_H*/