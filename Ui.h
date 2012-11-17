/*
	UI Main Module

	The UI works like this:
		- The main module is initialised
		- From there, the user launches modules
		- These modules will perform tasks and may even launch other modules either through "API" or other means
		- When modules end, they should return their functions.
		- Exceptions not cuaght and cascaded to the main module will result in fatal error and exit
*/
#pragma once
#ifndef Ui_H
#define Ui_H

#include <string>
#include <sstream>
#include "WaveFile.h"
#include "Exception.h"
#include "engine.h"			//Matlab

namespace Ui{

	/*********************
		UI "API"
	***********************/
	void Initialise();					//Initialise the UI and asks for user input
	void LaunchModule(void (*method)(std::string arg), std::string arg, std::string ID);		//Launch a module and update the "Environment" string

	/**
		Constants
	**/
	//const unsigned int BUFFER_MAGIC_FACTOR = 1000U;		//Define the number of operations done to the buffer first before flushing it
	const std::size_t BUFFER_SIZE = 1048576;			//Set the buffer size, in byte for output. 1048576 = 1MB

	/**
		UI Utility
	**/
	void Continue();		//Prompt the user to press enter to continue
	void OutputLine();		//Output a line
	
	//Get a line from cin and returns in a stringstream object
	//Set AllowEmpty to false to wait until a non empty input is retrieved
	std::stringstream GetInputLine(bool AllowEmpty=false, bool ShowEnvironment=true);		

	/**
		Error Handling Functions
	**/
	/********
		Dumps a Fatal Exception or an exception that was not caught. Behaviour depends on if debugging mode is on.
		If Debugging mode is on, the exception error is dumped to cout
		Along with the calling stack to the debugger output
		Otherwise a "user friendly" message is dumped to the cout
	*******/
	void DumpFatalException(Exception &e);	
	void DumpFatalException(std::exception &e);	

	/**
		File Dumping Functions
	**/
	/********
		When using fstream, std::endl will insert a new line and flush the stream
		c.f. http://stackoverflow.com/questions/5192239/why-is-ostringstream-faster-than-ofstream

		In order to allow endl usage, an additional buffer is used.
		Stream flushing and file writing helper function
		Looks at the total number of operation, n. If it sees a suitable amount of operations, k, has been written to
		the buffer, it will write the bufer and flush the buffer
		k will be incremented everything this function is called
	*********/
	//void BufferFlushHelper(ostream &output, stringstream &buffer, unsigned int &k, const unsigned int n, const bool forced=false);

	//Dump a Time Domain analysis of a wave file complete with some meta data
	//Overwrites existing files
	//WARNING: THIS FUNCTION IS SLOW, ESPECIALLY FOR HUGE FILES
	void DumpWaveTimeDomain(Wave::WaveFile &input, const char *dump);	

	/***********
		MatLab
	**********/
	struct Matlab_T{
		Engine *engine;
		Engine *operator()();
		Matlab_T(): engine(NULL){ }
		~Matlab_T() { if (engine) engClose(engine); }
		void MakeVisible(){
			engSetVisible(this->operator()(), true);
		}
		void MakeInvisible(){
			engSetVisible(this->operator()(), false);
		}
	};

	//Module Type
	//"Module"
	struct Module_T{
		std::string ID;			//The "ID" used to launch the module
		std::string Name;		//Name of the module
		std::string Description;	//Description
		void (*Method)(std::string arg);		//Pointer to the method to launch for the module

		//Initialiser
		Module_T(std::string id="", std::string name="", std::string description="", void (*method)(std::string arg)=NULL)
			: ID(id), Name(name), Description(description), Method(method){
		} //...
	};


}

#endif /* Ui_H */