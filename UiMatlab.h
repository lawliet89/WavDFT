/*
	Matlab UI Module
*/

#pragma once
#ifndef UiMatlab_H
#define UiMatlab_H

#include <string>
#include "DFTMatlab.h"
#include "DFTData.h"

namespace Ui{
	//Data Structure
	struct MatlabData_T{
		DFT::DFTTime *T;				//Ptr to time data obj
		DFT::DFTFrequency *F;			//Ptr to freq data obj
		DFT::DFTMatlab *M;				//Ptr to matlab transform obj
		bool IsPreset;					//Is Preset?
		bool ListCmd;

		MatlabData_T(): T(NULL), F(NULL), M(NULL), IsPreset(false), ListCmd(true) {}
		~MatlabData_T(){
			if (!IsPreset){
				delete T;
				delete F;
			}
			delete M;
		}
	};

	//Matlab Modules
	struct MatlabModule_T{
		std::string ID;			//The "ID" used to launch the module
		std::string Name;		//Name of the module
		std::string Description;	//Description
		void (*Method)(std::string arg, MatlabData_T &MatlabData);		//Pointer to the method to launch for the module

		//Initialiser
		MatlabModule_T(std::string id="", std::string name="", std::string description="", void (*method)(std::string arg, MatlabData_T &MatlabData)=NULL)
			: ID(id), Name(name), Description(description), Method(method){
		} //...
	};

	//Initialise Data
	void MatlabInit(MatlabData_T &MatlabData);						//Initialise module list and variables for the current session
	void MatlabInitMat(MatlabData_T &MatlabData);					//Initialisation for Matlab and DFTMatlab object

	//MODULES
	void MatlabMain(std::string arg);								//"Main module"
	void MatlabHelp(std::string arg, MatlabData_T &MatlabData);		//Help
	void MatlabFFT(std::string arg, MatlabData_T &MatlabData);		//Perform FFT
	void MatlabIFFT(std::string arg, MatlabData_T &MatlabData);		//Perform IFFT
	void MatlabSend(std::string arg, MatlabData_T &MatlabData);		//Send variable to matlab
	void MatlabGet(std::string arg, MatlabData_T &MatlabData);		//Get variable from Matlab
	void MatlabOpen(std::string arg, MatlabData_T &MatlabData);		//Matlab Open
	void MatlabClose(std::string arg, MatlabData_T &MatlabData);	//Close Matlab
	void MatlabStatus(std::string arg, MatlabData_T &MatlabData);	//Status of T & F variables
	void MatlabWave(std::string arg, MatlabData_T &MatlabData);		//Create a Wave object and launch the Wave module
	void MatlabPlot(std::string arg, MatlabData_T &MatlabData);		//Plot data

	//Overload Launch Module
	void LaunchModule(void (*method)(std::string arg, MatlabData_T &MatlabData), std::string arg, MatlabData_T &MatlabData, std::string ID);
}

#endif /*UiMatlab_H*/