#include "DFTMatlab.h"
//#include <iostream>

namespace DFT{
	//Perform Discrete Fourier Transform
	void DFTMatlab::DiscreteFourierTransform(){
		//cf http://www.mathworks.com/help/techdoc/apiref/bqoqnz0.html
		mxArray *T = NULL , *F = NULL;			//Time and Frequency Domain Arrays
		unsigned intervaln = TimeDomain -> DFTNumInterval();
		unsigned dimension = TimeDomain -> DFTDimension();

		//To prevent memory leaks, we better wrap everything around a try block. Just in case the data types throw exceptions
		try{
			//Create the matrices
			T = mxCreateDoubleMatrix(intervaln, dimension, mxCOMPLEX);
			//F = mxCreateDoubleMatrix(interval, dimension, mxCOMPLEX);
			if (!T){
				throw Exception(EXCEPTION_MEMORY_ERROR, "Could not allocate memory for Matrices");
			}

			//Get pointer to the "real array" of T
			double *TReal = mxGetPr(T);
			//Pointer to the "imaginary array" of T
			double *TIm = mxGetPi(T);
   
			//Populate T
			//Note: The array returned above works like this: It lists all the items in one column first, then concatenates the second column after the first column
			//And so on
			// i.e. for a 2x2 matrix, the pointers go like this: (matrix coordinates column by row - x,y)
			// 1,1 -> 1,2 -> 2,1 -> 2,2
			for (unsigned j = 0; j < dimension; j++){
				for (unsigned i = 0; i < intervaln; i++){
					complex<double> num = TimeDomain->DFTGet(i, j);
					//std::cout << i << " " << j << " " << num.real() << endl;
					//engPutVariable(Matlab, "T", T);
					*(TReal + j*intervaln+i) = num.real();
					*(TIm + j*intervaln+i) = num.imag();
				}
			}

			//Put variable in workspace
			engPutVariable(Matlab, "T", T);  

			//Let's do FFT
			engEvalString(Matlab, "F = fftn(T)");

			//Get F back from workspace
			F = engGetVariable(Matlab, "F");

			//Save it into FrequencyDomain
			//Get pointer to the "real array" of T
			TReal = mxGetPr(F);
			//Pointer to the "imaginary array" of T
			TIm = mxGetPi(F);
			
			//We might have to change the dimensions and intervaln of  domain - be sure to catch exceptions
			if (dimension != FrequencyDomain->DFTDimension()){
				FrequencyDomain->DFTSetDimension(dimension);
			}
			if (intervaln != FrequencyDomain->DFTNumInterval()){
				FrequencyDomain->DFTSetNumInterval(intervaln);
			}

			for (unsigned j = 0; j < dimension; j++){
				for (unsigned i = 0; i < intervaln; i++){
					complex<double> num(*(TReal + j*intervaln+i),0);
					if (TIm){
						num.imag(*(TIm + j*intervaln+i));
					}
					FrequencyDomain->DFTSet(i,j,num);
				}
			}
		}
		catch(...){
			if (T){
				mxDestroyArray(T);
			}
			if (F){
				mxDestroyArray(F);
			}
			//Rethrow
			throw;
		}

		//Free memory
		if (T){
			mxDestroyArray(T);
		}
		if (F){
			mxDestroyArray(F);
		}
	}

	//Inverse Fourier Transform
	void DFTMatlab::InverseDiscreteFourierTransform(){
		//cf http://www.mathworks.com/help/techdoc/apiref/bqoqnz0.html
		mxArray *T = NULL , *F = NULL;			//Time and Frequency Domain Arrays
		unsigned intervaln = FrequencyDomain -> DFTNumInterval();
		unsigned dimension = FrequencyDomain -> DFTDimension();

		//To prevent memory leaks, we better wrap everything around a try block. Just in case the data types throw exceptions
		try{
			//Create the matrices
			//T = mxCreateDoubleMatrix(intervaln, dimension, mxCOMPLEX);
			F = mxCreateDoubleMatrix(intervaln, dimension, mxCOMPLEX);
			if (!F){
				throw Exception(EXCEPTION_MEMORY_ERROR, "Could not allocate memory for Matrices");
			}

			//Get pointer to the "real array" of F
			double *TReal = mxGetPr(F);
			//Pointer to the "imaginary array" of F
			double *TIm = mxGetPi(F);
   
			//Populate F
			//Note: The array returned above works like this: It lists all the items in one column first, then concatenates the second column after the first column
			//And so on
			// i.e. for a 2x2 matrix, the pointers go like this: (matrix coordinates column by row - x,y)
			// 1,1 -> 1,2 -> 2,1 -> 2,2
			for (unsigned j = 0; j < dimension; j++){
				for (unsigned i = 0; i < intervaln; i++){
					complex<double> num = FrequencyDomain->DFTGet(i, j);
					//std::cout << i << " " << j << " " << num.real() << endl;
					//engPutVariable(Matlab, "T", T);
					*(TReal + j*intervaln+i) = num.real();
					*(TIm + j*intervaln+i) = num.imag();
				}
			}

			//Put variable in workspace
			engPutVariable(Matlab, "F", F);  

			//Let's do iFFT
			engEvalString(Matlab, "T = ifftn(F)");

			//Get T back from workspace
			T = engGetVariable(Matlab, "T");

			//Save it into TimeDomain
			//Get pointer to the "real array" of T
			TReal = mxGetPr(T);
			//Pointer to the "imaginary array" of T
			TIm = mxGetPi(T);
   
			//We might have to change the dimensions and intervaln of  domain - be sure to catch exceptions
			if (dimension != TimeDomain->DFTDimension()){
				TimeDomain->DFTSetDimension(dimension);
			}
			if (intervaln != TimeDomain->DFTNumInterval()){
				TimeDomain->DFTSetNumInterval(intervaln);
			}

			for (unsigned j = 0; j < dimension; j++){
				for (unsigned i = 0; i < intervaln; i++){
					complex<double> num(*(TReal + j*intervaln+i),0);
					if (TIm){
						num.imag(*(TIm + j*intervaln+i));
					}
					TimeDomain->DFTSet(i,j,num);
				}
			}
		}
		catch(...){
			if (T){
				mxDestroyArray(T);
			}
			if (F){
				mxDestroyArray(F);
			}
			//Rethrow
			throw;
		}

		//Free memory
		if (T){
			mxDestroyArray(T);
		}
		if (F){
			mxDestroyArray(F);
		}
	}
}