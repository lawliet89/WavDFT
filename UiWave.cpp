//Wave UI Module
#include "Ui.h"
#include "UiWave.h"
#include "UiMatlab.h"
#include <iostream>
#include <vector>
#include <map>
#include <new>

using namespace std;

//If you want to call this module with preset data, set the preset variables below to something
namespace Ui{
	//extern Matlab_T Matlab;		//Matlab engine
	extern vector<string> Environment;
	extern DFT::DFTTime *PresetT;
	extern DFT::DFTFrequency *PresetF;

	Wave::WaveFile *PresetWave;				//Preset BOTH to something to use preset pointers
	DFT::DFTGenericFrequency *PresetFreq;

	//Declare "SubModule"
	map<string, WaveModule_T> WaveMods; 

	void LaunchModule(void (*method)(std::string arg, WaveData_T &WaveData), std::string arg, WaveData_T &WaveData, std::string ID){
		//Environment.push_back(ID);
		method(arg, WaveData);
		//Environment.pop_back();
	}
	void WaveInit(WaveData_T &WaveData){
		static bool init = false;
		if (init == false){
			//Help
			WaveMods["help"] = WaveModule_T("help", "Help", "Get help details about other commands.\nUsage:\n\thelp cmd\nwhere cmd is the command to look for information about.\nType help to show the list of commands.", &WaveHelp);
			//Quit - "special"
			WaveMods["quit"] = WaveModule_T("quit", "Quit", "Quit the Wave Tool");
			//Open
			WaveMods["open"] = WaveModule_T("open", "Open", "Open a Wave File and then parse the file. \nUsage:\n\topen file\nwhere file is the path to the file to open.\nWARNING: If a file was open or data was present, it will be overwritten.", &WaveOpen);
			//Matlab
			WaveMods["matlab"] = WaveModule_T("matlab","Matlab Tool", "Load current Time Domain (Wave File data) and Frequency Domain Data into Matlab and launch the Matlab Module.\nWARNING: Any edits you make to the data will be reflected back in this module. You cannot change the number of blocks or channels. If you want to create a new file from this wave as a basis, use copy.", &WaveMatlab);
			//Info
			WaveMods["info"] = WaveModule_T("info", "Display Information", "Display information about the Wave file and the Frequency domain data object", &WaveInfo);
			//Copy
			WaveMods["copy"] = WaveModule_T("copy","Copy to Matlab", "Copy current Time Domain (Wave File data) and Frequency Domain Data into Matlab and launch the Matlab Module.\nAny edits in Matlab will be LOST when Matlab is closed. Use this as a basis to create new wave file from this wave. Use 'wave' from Matlab to generate a new WaveFile.", &WaveCopy);
			//Dump
			WaveMods["dump"] = WaveModule_T("dump", "Dump Wave File", "Based on the current stream data, dump the data to a CSV. \nUsage:\n\tdump file\nwhere file is the filename to dump the CSV to.\nWARNING: If the file specified already exist, it will be overwritten.", &WaveDump);
			//Load
			WaveMods["load"] = WaveModule_T("load", "Load Data", "Load data into memory for faster access.", &WaveLoad);
			//Unload
			WaveMods["unload"] = WaveModule_T("unload", "Unload Data", "Unload any data in memory. This CLEARS all data in memory and if they were not saved, they will be lost", &WaveLoad);
			//Write
			WaveMods["write"] = WaveModule_T("write", "Write Wave File", "Based on the data contained in memory, write to a wave file.\nUsage\n\twrite file\nwhere file is the path to the file to write.", &WaveWrite);
			init = true;
		}
		if(PresetWave && PresetFreq){
			WaveData.Wav = PresetWave;
			PresetWave = NULL;
			WaveData.Freq = PresetFreq;
			PresetFreq = NULL;

			WaveData.IsPreset = true;
		}
		else{
			//Create objects
			WaveData.Wav = new (nothrow) Wave::WaveFile();
			//WaveData.Freq = new (nothrow) DFT::DFTGenericFrequency();
			if (!WaveData.Wav){
				delete WaveData.Wav;
				delete WaveData.Freq;
				throw Exception(EXCEPTION_MEMORY_ERROR, "Memory allocation error");
			}
		}
	}

	//Main Module
	void WaveMain(string arg){
		WaveData_T WaveData;		
		try{
			WaveInit(WaveData);	//Initialise
			while(1){			//Launch into an infinite loop until user breaks out of it
				if (WaveData.ListCmd){		//List the commands available
					OutputLine();
					cout << "Welcome to the Wave Tool.\n"
						<< "The following commands are available. \nType 'help command' for help on a particular command\n";
					unsigned char count = 0;
					map<string, WaveModule_T>::iterator it;
					for (it = WaveMods.begin(); it != WaveMods.end(); it++){
						cout << "\t" << it->second.ID;
						count++;
						if (count == 4){
							cout << '\n';
							count = 0;
						}
					}
					if (count){
						cout << '\n';
					}
					OutputLine();
					WaveData.ListCmd = false;
				}
				stringstream line = GetInputLine();		//Get input
				string cmd;
				line >> cmd;	//Get command from input
				if (cmd == "quit"){
					//Nice let's exit
					return;
				}
				else{
					map<string, WaveModule_T>::iterator it = WaveMods.find(cmd);		
					if (it == WaveMods.end()){				//Look for the command
						cout << "'" << cmd << "' is not a valid command. Please try again. \n";
						continue;
					}
					else{
						//Find arg
						string arg = "";
						if (line.str().length() > cmd.length()){
							arg = line.str().substr(cmd.length()+1, string::npos);
						}
						LaunchModule(it->second.Method, arg, WaveData, it->second.ID);			//Launch it
						// =D
					}
				}
			}
		}
		catch(Exception &e){
			if (e.GetErrorCode() == EXCEPTION_MEMORY_ERROR){
				//Memory error... this can be fatal
				cout << "A memory allocation error has occurred. Have you run out of memory? This tool will now exit." << endl;
				//delete WaveData.Wav;
				//delete WaveData.Freq;
				return;
			}
		}
		catch(...){
			//Unknown exception. Let's take care of leaks
			//delete WaveData.Wav;
			//delete WaveData.Freq;
			throw;
		}
		//delete WaveData.Wav;
		//delete WaveData.Freq;
	}

	//Help Module
	void WaveHelp(string arg, WaveData_T &WaveData){
		if (arg.empty()){
			WaveData.ListCmd = true;			//Show list of commands again
			return;
		}
		map<string, WaveModule_T>::iterator it = WaveMods.find(arg);
		if (it == WaveMods.end()){
			cout << "'" << arg << "' is not a valid command. Please try again. \n";
		}
		else{
			cout << '\n' << it->second.Name;
			cout << '\n' << it->second.Description << "\n\n";
		}
	}

	//Open Wave
	void WaveOpen(string arg, WaveData_T &WaveData){
		if (arg.empty()){
			return LaunchModule(&WaveHelp, "open", WaveData, "help");
		}
		try{
			//Try to open file
			WaveData.Wav->Open(arg.c_str());

			//Parse the file
			WaveData.Wav->Parse();

			cout << "File opened and parsed. Use 'info' to display information about the file.\n";
		}
		catch(Exception &e){
			switch(e.GetErrorCode()){
			case EXCEPTION_FILE_CANNOT_OPEN:
				cout << "Could not open the file. Please try again. \n";
				break;
			case EXCEPTION_PARSE_FORMAT_ERROR:
			case EXCEPTION_PARSE_MISSING_TOKEN:
			case EXCEPTION_PARSE_MISSING_DATA:
				cout << "The file is corrupted. The error was: \n\t" << e.GetErrorMessage() << "\n Please try again.\n"; 
				break;
			default:
				cout << "An unknown error has occurred. The error was: \n\t" << e.GetErrorMessage() << "\n Please try again.\n"; 
				break;
			}

			//Clear object
			delete WaveData.Wav;
			WaveData.Wav = new(nothrow) Wave::WaveFile();
			if (!WaveData.Wav){
				throw Exception(EXCEPTION_MEMORY_ERROR, "Memory allocation error");
			}
		}
	}
	//Matlab module
	void WaveMatlab(string arg, WaveData_T &WaveData){
		cout << "WARNING: Any change you make in the Matlab module will cause a change in this module. "
			<< "You also cannot change the number of channels (dimensions) or blocks (interval number) in Matlab. "
			<< "If you want to create a new file from this wave as a basis, use copy.\n";
		Continue();
		//Check for data in Wave
		if (!WaveData.Wav->DataIsLoaded()){
			try{
				WaveData.Wav->DataLoad();
			}
			catch(Exception &e){
				if(e.GetErrorCode() == EXCEPTION_FILE_NOT_OPEN){
					cout << "Error: There is no file open and no data in the object. Create some data first or load a file.\n"
						<<"If you want to create data from scratch, launch the Matlab tool to create the data and invoke the wave command to create a"
						<< "wave object\n";
				}
				else{
					cout << "An unknown error has occurred: \n\t" << e.GetErrorMessage() << '\n';
				}
				return;
			}

		}
		if (!WaveData.Freq){			//Create empty
			WaveData.Freq = new (nothrow) DFT::DFTGenericFrequency(WaveData.Wav->DFTDimension(), WaveData.Wav->DFTInterval(), WaveData.Wav->DFTSample());
		}
		if (!WaveData.Freq ){
			cout << "Error, could not allocate memory to store Frequency Domain data\n";
			return;
		}
		PresetT = dynamic_cast<DFT::DFTTime*>(WaveData.Wav);				//Cast to allow for use in Matlab module
		PresetF = dynamic_cast<DFT::DFTFrequency*>(WaveData.Freq);
		if (!PresetT || !PresetF){
			cout << "Error, could not initialise the Matlab Module.\n";
			return;
		}
		LaunchModule(&MatlabMain, "", "matlab");
		cout << "WARNING: Any change you have made in the Matlab module will have caused a change in this module.\n";
		Continue();
	}

	//Info
	void WaveInfo(std::string arg, WaveData_T &WaveData){
		if (WaveData.IsPreset){
			cout << "Warning: Your data is preloaded. Changes you make here may or may not propogate back.\n0";
		}
		cout << "Wave File Details: \n";
		if (WaveData.Wav->IsOpen()){
			cout << "\t - File open and associated\n";
			if (WaveData.Wav->DataIsLoaded()){
				cout << "\t - Stream data loaded\n";
			}
			else{
				cout << "\t - Stream data not loaded\n";
			}
		}
		else{
			cout << "\t - No file associated\n";
			if (WaveData.Wav->DataIsLoaded()){
				cout << "\t - Stream data present\n";
			}
			else{
				cout << "\t - Stream data not present\n";
			}
		}
		cout << "\t - Data Stream Size: " << WaveData.Wav->DataChunkSize() << " Bytes\n";
		cout << "\t - Channel Size: " << WaveData.Wav->NumChannels() << "\n";
		cout << "\t - Sampling Rate: " << WaveData.Wav->SampleRate() << " Hz\n";
		cout << "\t - Sampling Interval: " << WaveData.Wav->Interval() << " s\n";
		cout << "\t - Block Size " << WaveData.Wav->BlockSize() << " Bytes\n";
		cout << "\t - Sample Size: " << WaveData.Wav->SampleSize() << " Bits\n";
		cout << "\t - Sample Count: " << WaveData.Wav->NumSamples() << "\n";
		cout << "\t - Block Count: " << WaveData.Wav->NumBlocks() << "\n";

		cout << "Frequency Domain Details: \n";
		if (!WaveData.Freq){
			cout << " - Not Loaded\n";
		}
		else{
			cout << "\t - Dimensions: " << WaveData.Freq->DFTDimension() << "\n";
			cout << "\t - Interval Count: " << WaveData.Freq->DFTNumInterval() << "\n";
			cout << "\t - Frequency Interval: " << WaveData.Freq->DFTInterval() << "\n";
			cout << "\t - Number of Samples: " << WaveData.Freq->DFTSample() << "\n";
		}
	}
	//Copy
	void WaveCopy(std::string arg, WaveData_T &WaveData){
		cout << "Copying data...\n";
		//Create objects
		DFT::DFTGenericTime *T = new(nothrow) DFT::DFTGenericTime(WaveData.Wav->DFTDimension(), WaveData.Wav->DFTInterval(), WaveData.Wav->DFTNumInterval());
		if (!T){
			cout << "Error: Could not copy Wave data.\n";
			return;
		}
		//Create object
		DFT::DFTGenericFrequency *F = NULL;
		if (WaveData.Freq){
			 F = new(nothrow) DFT::DFTGenericFrequency(WaveData.Freq->DFTDimension(), WaveData.Freq->DFTInterval(), WaveData.Freq->DFTNumInterval());
		}
		else{
			F = new(nothrow) DFT::DFTGenericFrequency(); 
		}

		if (!F){
			cout << "Error: Could not copy Frequency domain data.\n";
			return;
		}

		try{
			//Populate time data
			unsigned intervalN = WaveData.Wav->DFTNumInterval();
			unsigned dimension = WaveData.Wav->DFTDimension();
			for (unsigned i = 0; i < intervalN; i++){
				for (unsigned j = 0; j < dimension; j++){
					T->DFTSet(i,j, WaveData.Wav->DFTGet(i,j));
				}
			}
			//Populate freq data
			if (WaveData.Freq){
				intervalN = WaveData.Freq->DFTNumInterval();
				dimension = WaveData.Freq->DFTDimension();
				for (unsigned i = 0; i < intervalN; i++){
					for (unsigned j = 0; j < dimension; j++){
						F->DFTSet(i,j, WaveData.Freq->DFTGet(i,j));
					}
				}
			}
			//Dynamic cast to allow for use in Matlab module
			PresetT = dynamic_cast<DFT::DFTTime*>(T);
			PresetF = dynamic_cast<DFT::DFTFrequency*>(F);
			if (!PresetT || !PresetF){
				cout << "Error, could not initialise the Matlab Module.\n";
				delete T;
				delete F;
				return;
			}
			LaunchModule(&MatlabMain, "", "matlab");
		}
		catch(...){			//In case of exceptions, we are not going to handle. But we are gonna prevent memory leaks
			delete T;
			delete F;
			throw;
		}
	}
	//Dump
	void WaveDump(std::string arg, WaveData_T &WaveData){
		if (arg.empty()){
			return LaunchModule(&WaveHelp, "dump", WaveData, "help");			//Show help
		}
		try{
			cout << "Dumping file... ";
			DumpWaveTimeDomain(*WaveData.Wav, arg.c_str());			//Simple dumping
			cout << "Done.\n";
		}
		catch(Exception &e){
			if (e.GetErrorCode() == EXCEPTION_FILE_CANNOT_OPEN_OUTPUT){
				cout << "Unable to open file for writing.\n";
				return;
			}
			else{
				throw;
			}
		}
	}
	//Load
	void WaveLoad(std::string arg, WaveData_T &WaveData){
		try{
			WaveData.Wav->DataLoad();
		}
		catch(Exception &e){
			if (e.GetErrorCode() == EXCEPTION_FILE_NOT_OPEN){
				cout << "Error. No file is associated. \n";
			}
		}
	}
	//Unload
	void WaveUnload(std::string arg, WaveData_T &WaveData){
		WaveData.Wav->DataUnload();
	}
	//Write
	void WaveWrite(std::string arg, WaveData_T &WaveData){
		if (arg.empty()){
			return LaunchModule(&WaveHelp, "write", WaveData, "help");
		}
		try{
			WaveData.Wav->WriteFile(arg.c_str());
			cout << "File written.\n";
		}
		catch (Exception &e){
			cout << "An error has occurred: " << e.GetErrorMessage() << "\n";
		}
	}
}