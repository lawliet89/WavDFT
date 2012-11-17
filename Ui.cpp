/*
	"Main" UI Module
*/
#include <iostream>
#include <vector>
#include "Ui.h"
#include "Exception.h"

//Module Headers to go here
#include "UiWave.h"
#include "UiMatlab.h"

using namespace std;

namespace Ui{
	//Environment String
	vector<string> Environment;
	//Declare Matlab Engine
	Matlab_T Matlab;

	/*
		A list of modules that the "main" module will "launch"
	*/

	//"Modules List"
	vector<Module_T> ModuleList;

	Engine *Matlab_T::operator()(){
		if (!engine){
			std::cout << "Starting Matlab... This might take a while...\n";
			engine = engOpen('\0');
			engSetVisible(engine, false);
		}
		return engine;
	}
	
	//Main Initialisation Function
	void Initialise(){
		//Initialise Modules List
		ModuleList.push_back(Module_T("wave", "Wave Tool", "Analyse and manipulate Wave Files", &WaveMain));
		ModuleList.push_back(Module_T("matlab", "Matlab Tool", "Matlab interface tool to generate FFT or manipulate data", &MatlabMain));

		//Begin UI
		cout << "**********************************\n"
			 << "Welcome to the Wave & DFT Application\n"
			 << "**********************************\n";
		try{
			bool ListMods = true;
			while(1){
				if (ListMods){
					OutputLine();
					cout<< "The following tasks are available. \nType the number of the task you would like to launch and press ENTER.\n"
						<< "\t0: Quit\n";
					//List Modules
					vector<Module_T>::iterator it;
					unsigned int i = 1;
					for(it = ModuleList.begin(); it < ModuleList.end(); it++, i++){
						Module_T current = *it;
						cout << "\t" << i << ": " << current.Name << "\n";
					}
					cout << "Type '?' followed by the number of the task to get a description.\n";
					OutputLine();
				}
				else{
					ListMods = true;
				}
				stringstream cmd = GetInputLine();
				if (cmd.str() == "0"){
					return;		//Exit. Thank you.
				}
				else{
					//User is asking for help
					if (cmd.peek() == (int) '?'){
						cmd.ignore();	//Discard ?
						int index = 0;
						cmd >> index;
						if (index <= 0 || index > ModuleList.size()){
							cout << "'" << index << "' is not a valid task. Please try again.\n";
							ListMods = false;
							continue;
						}
						else{
							OutputLine();
							Module_T current = ModuleList[index-1];
							cout << current.Name
								<< ":\n\t"
								<< current.Description
								<< "\n";
							OutputLine();
							ListMods = false;
							continue;
						}
					}
					else{
						int index = 0;
						cmd >> index;
						if (index <= 0 || index > ModuleList.size()){
							cout << "'" << index << "' is not a valid task. Please try again.\n";
							ListMods = false;
							continue;
						}
						else{
							//Launch the thing!
							Module_T current = ModuleList[index-1];
							//Find arg
							string arg = "";
							if (cmd.str().length() > 1){
								arg = cmd.str().substr(1, string::npos);
							}

							LaunchModule(current.Method, arg, current.ID);
							//After the module runs its course, we will resume from there.
						}
					}
				}
			}
		}
		catch(Exception &e){
			DumpFatalException(e);
		}
		catch(exception &e){
			//.. Hmm something we didn't expect
			DumpFatalException(e);
		}
		//Continue();
	}

	//LaunchModule()
	void LaunchModule(void (*method)(std::string arg), std::string arg, std::string ID){
		Environment.push_back(ID);
		method(arg);
		Environment.pop_back();
	}

	/**
		UI Utility
	**/
	void Continue(){
		cout << "Press ENTER to continue...";
		cin.get();
		cin.clear();
	}
	//GetInputLine()
	stringstream GetInputLine(bool AllowEmpty, bool ShowEnvironment){
		string input;
		do{
			if (Environment.size() && ShowEnvironment){
				vector<string>::iterator it;
				for (it = Environment.begin(); it < Environment.end(); it++){
					cout << *it << "> ";
				}
			}
			else{
				cout << "> ";
			}
			getline(cin,input);
		}while(input.empty() && !AllowEmpty);
		return stringstream(input);
	}
	//OutputLine()
	void OutputLine(){
		cout << "---------------------------------\n";
	}

	/**
		Error Handling Functions
	**/
	//DumpException()
	void DumpFatalException(Exception &e){
#ifndef NDEBUG			//Debugging mode
		e.GetCallStack().ShowCallstack();
		cout << "An uncaught or fatal exception has ocurred. Details follow: " << endl;
		cout << "Error Code: " << hex << e.GetErrorCode() << endl;
		cout << "Error Message: " << e.GetErrorMessage() << endl;
		cout << "Extra Code: " << hex << e.GetExtraCode() << endl;
		cout << "Extra Message: " << e.GetExtraMessage() << endl;
		if (IsDebuggerPresent()){
		cout << "See debugger for the call stack." << endl;
			DebugBreak();				//Trigger a debug break
		}
		else{
			Continue();
		}
#else /*NDEBUG*/		//Production Mode
		cout << "An error that the program cannot recover from has ocurred." << endl;
		cout << "The error was \"" << e.GetErrorMessage() << "\"." << endl;
		cout << "The program will now terminate." << endl;
		Continue();

#endif /*MDEBUG*/
		//Terminate
		exit(1);
	}
	//DumpException()
	void DumpFatalException(std::exception &e){
#ifndef NDEBUG			//Debugging mode
		cout << "An uncaught or fatal exception has ocurred. Details follow: " << endl;
		cout << "Error: " << e.what() << endl;
		if (IsDebuggerPresent()){
			DebugBreak();				//Trigger a debug break
		}
		else{
			Continue();
		}
#else /*NDEBUG*/		//Production Mode
		cout << "An error that the program cannot recover from has ocurred." << endl;
		cout << "The error was \"" << e.what() << "\"." << endl;
		cout << "The program will now terminate." << endl;
		Continue();

#endif /*MDEBUG*/
		//Terminate
		exit(1);
	}

	/**
		File Dumping Functions
	**/
	//BufferFlushHelper()
/*	void BufferFlushHelper(ostream &output, stringstream &buffer, unsigned int &k, const unsigned n, const bool forced){
		if (output.fail()){
			RaiseWarning(L"Output file is not in a ready state.");
			return;
		}
		if (k == n || forced || (k % BUFFER_MAGIC_FACTOR == 0 && k != 0) ){				//The end
			//Flush.
			output << buffer.str();	//Write to file
			buffer.str("");		//Clear string buffer
			k = 0;
		}
		else{
			k++;
		}
	}*/
	//DumpWaveTimeDomain()
	void DumpWaveTimeDomain(Wave::WaveFile &input, const char *dump){
		ofstream file(dump, ios_base::out | ios_base::trunc);
		//stringstream buffer;			//Buffer used to store input to improve performance at the cost of overhead
		//unsigned int k = 0;					//Used to flush and write buffer
		//unsigned int n = input.NumBlocks();

		if (!file.is_open()){
			throw Exception(EXCEPTION_FILE_CANNOT_OPEN_OUTPUT,"Unable to open file for output");
		}

		//Set a larger buffer - cf http://stackoverflow.com/questions/5192239/why-is-ostringstream-faster-than-ofstream
		char *buffer = NULL;
		try{
			buffer = new char[BUFFER_SIZE];
			file.rdbuf()->pubsetbuf(buffer, BUFFER_SIZE);
		}
		catch(bad_alloc){
			RaiseWarning(L"Buffer was set too large. Default buffer size used.");
		}
		
		try{		//The following code should not throw exceptions but let's be careful. The memory leak can be HUGE
			//Input MetaData
			/*
				MetaData line format:
					NumerOfChannels,SampleRate,SampleBitRate
			*/
			file << input.NumChannels() << ',' << input.SampleRate() 
				<< ',' << input.SampleSize() << '\n';

			input.DataRewind();

			//One block per line. Each channel seperated by a ,
			if (input.SampleSize() > 8){
				Wave::WaveBlock<int> Block;
				while (!input.DataEnd()){
					Block = input.DataNextBlock();
					for (unsigned int i = 0; i < input.NumChannels(); i++){
						file << Block.GetChannel(i) << ',';
					}
					file << '\n';			//Instead of std::endl. cf http://stackoverflow.com/questions/5192239/why-is-ostringstream-faster-than-ofstream
					//BufferFlushHelper(file,buffer,k,n);
				}
				//BufferFlushHelper(file,buffer,k,n, true);
			}
			else{
				Wave::WaveBlock<unsigned int> Block;
				while (!input.DataEnd()){
					Block = input.DataNextBlockUnsigned();
					for (unsigned int i = 0; i < input.NumChannels(); i++){
						file << Block.GetChannel(i) << ' ';
					}
					file << '\n';
					//BufferFlushHelper(file,buffer,k,n);
				}
				//BufferFlushHelper(file,buffer,k,n, true);
			}
		}
		catch(...){
			//Let's flush the file.
			file.flush();
			file.close();
			delete[] buffer;
			throw;
		}
		file.close();
		delete[] buffer;
	}
}
