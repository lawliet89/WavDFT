/*
	Wave UI Module
*/

#pragma once
#ifndef UiWave_H
#define UiWave_H
#include <string>
#include "WaveFile.h"
#include "DFTGeneric.h"

namespace Ui{
	//Data for each execution. Kinda like a "stack"
	struct WaveData_T{
		bool ListCmd;		//List items, or not?
		//Objects
		Wave::WaveFile *Wav;
		DFT::DFTGenericFrequency *Freq;
		bool IsPreset;
		WaveData_T(): ListCmd(true), Wav(NULL), Freq(NULL), IsPreset(false) {}
		~WaveData_T(){
			if (!IsPreset){
				delete Wav;
				delete Freq;
			}
		}
	};

	struct WaveModule_T{
		std::string ID;			//The "ID" used to launch the module
		std::string Name;		//Name of the module
		std::string Description;	//Description
		void (*Method)(std::string arg, WaveData_T &WaveData);		//Pointer to the method to launch for the module

		//Initialiser
		WaveModule_T(std::string id="", std::string name="", std::string description="", void (*method)(std::string arg, WaveData_T &WaveData)=NULL)
			: ID(id), Name(name), Description(description), Method(method){
		} //...
	};
	//Initiaiser
	void WaveInit();													//Initialise module list and session variables
	
	//UI Wave Modules
	void WaveMain(std::string arg);										//The "main" method
	void WaveHelp(std::string arg, WaveData_T &WaveData);				//Help
	void WaveOpen(std::string arg, WaveData_T &WaveData);				//Open and intepret file
	void WaveMatlab(std::string arg, WaveData_T &WaveData);				//Matlab. Load variables and launch Matlab module
	void WaveCopy(std::string arg, WaveData_T &WaveData);				//Matlab. Copy variables and launch Matlab module
	void WaveInfo(std::string arg, WaveData_T &WaveData);				//Show info of time/freq
	void WaveFmt(std::string arg, WaveData_T &WaveData);				//Edit fmt chunk
	void WaveWrite(std::string arg, WaveData_T &WaveData);				//Write wave file
	void WaveDump(std::string arg, WaveData_T &WaveData);				//Dump wave file time domain
	void WaveLoad(std::string arg, WaveData_T &WaveData);				//Load data into memory
	void WaveUnload(std::string arg, WaveData_T &WaveData);				//Unload

	//Overload Launch Module
	void LaunchModule(void (*method)(std::string arg, WaveData_T &WaveData), std::string arg, WaveData_T &WaveData, std::string ID);
}

#endif /*UiWave_H*/