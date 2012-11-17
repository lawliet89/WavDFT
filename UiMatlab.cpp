//Matlab UI Module
#include <iostream>
#include <map>

#include "Ui.h"
#include "UiMatlab.h"
#include "DFTGeneric.h"
#include "WaveFile.h"
#include "UiWave.h"

using namespace std;

namespace Ui{
	extern Matlab_T Matlab;		//Matlab engine
	extern vector<string> Environment;
	extern Wave::WaveFile *PresetWave;
	extern DFT::DFTGenericFrequency *PresetFreq;

	//Preset Variables
	DFT::DFTTime *PresetT;
	DFT::DFTFrequency *PresetF;


	//Declare "SubModule"
	map<string, MatlabModule_T> MatMods; 

	//Initialise
	void MatlabInit(MatlabData_T &MatlabData){
		static bool init = false;
		if (!init){
			//Initialise Module List
			//Quit - "special"
			MatMods["quit"] = MatlabModule_T("quit", "Quit", "Quit the Tool");
			//Help
			MatMods["help"] = MatlabModule_T("help", "Help","Get help details about other commands.\nUsage:\n\thelp cmd\nwhere cmd is the command to look for information about.\nType help to show the list of commands.", &MatlabHelp);
			//Status
			MatMods["status"] = MatlabModule_T("status", "Display Status","Displays the status regarding T & F - the time and frequency domain variables.", &MatlabStatus);
			//Send
			MatMods["send"] = MatlabModule_T("send", "Send Variable to Matlab Workspace","Send T & F to the Matlab Workspace.\nUsage\n\tsend type name\nwhere type is either 't' of 'f' representing the time and frequency domain respectively.\nname is the name of the variable to appear in Matlab Workspace", &MatlabSend);
			//Get
			MatMods["get"] = MatlabModule_T("get", "Get Variable from Matlab Workspace","Get T & F from the Matlab Workspace.\nUsage\n\tget type name\nwhere type is either 't' of 'f' representing the time and frequency domain respectively.\nname is the name of the variable to appear in Matlab Workspace\n\nWARNING: If your data was preset, this change might propogate to your calling module. Also, your calling module might not support changing the interval count or the dimension of the data. Use with care.", &MatlabGet);
			//FFT
			MatMods["fft"] = MatlabModule_T("fft", "Perform Fast Fourier Transform","Perform FFT of T and save the result in F. \n\nWARNING: If your data was preloaded, this operation might cause a change in F that will propogate to your calling module. If you simply wish to perform FFT without any change in F, send the variable to Matlab workspace and perform it in Matlab.", &MatlabFFT);
			//IFFT
			MatMods["ifft"] = MatlabModule_T("ifft", "Perform Inverse Fast Fourier Transform","Perform Inverse FFT of F and save the result in T. \n\nWARNING: If your data was preloaded, this operation might cause a change in T that will propogate to your calling module. If you simply wish to perform Inverse FFT without any change in F, send the variable to Matlab workspace and perform it in Matlab.", &MatlabIFFT);
			//Open
			MatMods["open"] = MatlabModule_T("open", "Open Matlab Command Window","Open Matlab command window for more options to manipulate data.", &MatlabOpen);
			//Close
			MatMods["close"] = MatlabModule_T("close", "Close Matlab Command Window","", &MatlabClose);
			//Plot
			MatMods["plot"] = MatlabModule_T("plot", "Plot Responses","Plot the frequency and time domain responses of the variables in Matlab for a particular dimension.\nUsage\n\tplot dimension\nwhere dimension is the dimension to plot data for.", &MatlabPlot);
			//Wave
			MatMods["wave"] = MatlabModule_T("wave", "Copy Data to Wave Module","Based on the data in T, create a wave file object and launch the Wave tool.\nNote: Any modification you make in Wave WILL NOT be saved in Matlab.", &MatlabWave);
			init = true;
		}

		if (PresetT && PresetF){
			MatlabData.T = PresetT;
			PresetT = NULL;
			MatlabData.F = PresetF;
			PresetF = NULL;
			MatlabData.IsPreset = true;
			
		}
		else{
			MatlabData.T = new DFT::DFTGenericTime();
			MatlabData.F = new DFT::DFTGenericFrequency();
		}
	}
	//Intialise DFT Object
	void MatlabInitMat(MatlabData_T &MatlabData){
		if (!MatlabData.M){
			MatlabData.M = new DFT::DFTMatlab(MatlabData.T,MatlabData.F, Matlab());
		}
	}

	//Overload Launch Module
	void LaunchModule(void (*method)(std::string arg, MatlabData_T &MatlabData), std::string arg, MatlabData_T &MatlabData, std::string ID){
		//Environment.push_back(ID);
		method(arg, MatlabData);
		//Environment.pop_back();
	}
	//Main Module
	void MatlabMain(std::string arg){
		MatlabData_T MatlabData;		
		try{
			MatlabInit(MatlabData);
			while(1){
				if (MatlabData.ListCmd){
					OutputLine();
					cout << "Welcome to the Matlab Tool.\n" << "F represents the Frequency Domain data and T represents the Time Domain data\n"
						<< "Use status to check on the status of these two variables\n"
						<< "The following commands are available. \nType 'help command' for help on a particular command\n";
					unsigned char count = 0;
					map<string, MatlabModule_T>::iterator it;
					for (it = MatMods.begin(); it != MatMods.end(); it++){
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
					MatlabData.ListCmd = false;
				}
				stringstream line = GetInputLine();
				string cmd;
				line >> cmd;	//Get command
				if (cmd == "quit"){
					//Nice let's exit
					return;
				}
				else{
					map<string, MatlabModule_T>::iterator it = MatMods.find(cmd);
					if (it == MatMods.end()){
						cout << "'" << cmd << "' is not a valid command. Please try again. \n";
						continue;
					}
					else{
						//Find arg
						string arg = "";
						if (line.str().length() > cmd.length()){
							arg = line.str().substr(cmd.length()+1, string::npos);
						}
						LaunchModule(it->second.Method, arg, MatlabData, it->second.ID);
						// =D
					}
				}
			}
		}
		catch(Exception &e){
			if (e.GetErrorCode() == EXCEPTION_MEMORY_ERROR){
				//Memory error... this can be fatal
				cout << "A memory allocation error has occurred. Have you run out of memory? This tool will now exit." << endl;
				return;
			}
		}
		catch(...){
			//Unknown exception. 
			throw;
		}
	}

	//Help
	void MatlabHelp(std::string arg, MatlabData_T &MatlabData){
		if (arg.empty()){			//Display more help later
			MatlabData.ListCmd = true;			//Show list of commands again
			return;
		}
		map<string, MatlabModule_T>::iterator it = MatMods.find(arg);
		if (it == MatMods.end()){
			cout << "'" << arg << "' is not a valid command. Please try again. \n";
		}
		else{
			cout << '\n' << it->second.Name;
			cout << '\n' << it->second.Description << "\n\n";
		}
	}

	//Status
	void MatlabStatus(std::string arg, MatlabData_T &MatlabData){
		OutputLine();
		cout << "Matlab Variable Status\n";
		if (MatlabData.IsPreset){
			cout << "WARNING: The variables are pre-loaded from another module. You may not be able to change some properties of the data.\n";
			cout << "Changes you make here in this module might propogate back to the module that called this module.\n" << "\n";
		}
		cout << "Time Domain Variable: \n";
		if(MatlabData.T){
			cout << "\tDimensions: " << MatlabData.T->DFTDimension() << "\n";
			cout << "\tInterval Count: " << MatlabData.T->DFTNumInterval() << "\n";
			cout << "\tTime Interval: " << MatlabData.T->DFTInterval() << "\n";
			cout << "\tNumber of Samples: " << MatlabData.T->DFTSample() << "\n";
		}
		else{
			cout << "\tNot Set.\n";
		}
		cout << "Frequency Domain Variable: \n";
		if(MatlabData.F){
			cout << "\tDimensions: " << MatlabData.F->DFTDimension() << "\n";
			cout << "\tInterval Count: " << MatlabData.F->DFTNumInterval() << "\n";
			cout << "\tFrequency Interval: " << MatlabData.F->DFTInterval() << "\n";
			cout << "\tNumber of Samples: " << MatlabData.F->DFTSample() << "\n";
		}
		else{
			cout << "\tNot Set.\n";
		}
		OutputLine();
	}

	//Send
	void MatlabSend(std::string arg, MatlabData_T &MatlabData){
		stringstream cmd = stringstream(arg);
		char var;
		cmd >> var;
		DFT::DFTData *data = NULL;
		switch (var){
			case 'f':
				data = MatlabData.F;
				break;
			case 't':
				data = MatlabData.T;
				break;
			default:
				cout << "Invalid data type. Please see help for more information \n";
				return;
		}
		//Get name
		string name;
		cmd >> name;
		if (name.empty()){
			cout << "Please enter a valid variable name. \n";
			return;
		}

		//Construct the data
		mxArray *M = NULL;
		unsigned intervaln = data->DFTNumInterval();
		unsigned dimension = data->DFTDimension();
		M = mxCreateDoubleMatrix(intervaln, dimension, mxCOMPLEX);
		if (!M){
			cout << "Error: Unable to allocate memory to create Matlab Matrix.\n";
			return;
		}
		cout << "Generating Matlab Matrix... \n";
		//Get pointer to the "real array" of M
		double *TReal = mxGetPr(M);
		//Pointer to the "imaginary array" of M
		double *TIm = mxGetPi(M);

		//Populate M
		for (unsigned j = 0; j < dimension; j++){
			for (unsigned i = 0; i < intervaln; i++){
				complex<double> num = data->DFTGet(i, j);
				//std::cout << i << " " << j << " " << num.real() << endl;
				//engPutVariable(Matlab, "T", T);
				*(TReal + j*intervaln+i) = num.real();
				*(TIm + j*intervaln+i) = num.imag();
			}
		}
		cout << "Transferring to Matlab Workspace... \n";
		//Put variable in workspace
		engPutVariable(Matlab(), name.c_str(), M);  
		Matlab.MakeVisible();
		//Destroy variable
		mxDestroyArray(M);
		cout << "Done.\n";
	}
	//Get
	void MatlabGet(std::string arg, MatlabData_T &MatlabData){
		stringstream cmd = stringstream(arg);
		char var;
		cmd >> var;
		DFT::DFTData *data = NULL;
		switch (var){					//Check for variable
			case 'f':
				data = dynamic_cast<DFT::DFTData*> (MatlabData.F);			//Dynamic Cast to the base class
				break;
			case 't':
				data = dynamic_cast<DFT::DFTData*> (MatlabData.T);
				break;
			default:
				cout << "Invalid data type. Please see help for more information \n";
				return;
		}
		//Get name
		string name;
		cmd >> name;
		if (name.empty()){
			cout << "Please enter a valid variable name. \n";
			return;
		}
		cout << "Retrieving data from Matlab...\n";
		//Get the data
		mxArray *M = NULL;
		M = engGetVariable(Matlab(), name.c_str());				//Get variable from Matlab
		if (!M){
			cout << "Error: Either the data does not exist in Matlab work space or there was a problem with not enough memory.\n";
			return;
		}

		unsigned intervaln = mxGetM(M);				//Get the number of rows
		unsigned dimension = mxGetN(M);				//Get number of columns

		//Check if intervaln and dimension match and if not, do the adjustment and check for exceptions
		//Some data types will not support these operations
		try{
			if (intervaln != data->DFTNumInterval()){
				data->DFTSetNumInterval(intervaln);
			}
		}
		catch(Exception e){
			if (e.GetErrorCode() == EXCEPTION_UNSUPPORTED){
				cout << "Your calling module does not support changing the number of intervals.\n";
				return;
			}
			else{
				throw;
			}
		}

		try{
			if (dimension != data->DFTDimension()){
				data->DFTSetDimension(dimension);
			}
		}
		catch(Exception e){
			if (e.GetErrorCode() == EXCEPTION_UNSUPPORTED){
				cout << "Your calling module does not support changing the number of dimensions.\n";
				return;
			}
			else{
				throw;
			}
		}
		
		cout << "Saving data...\n";
		//Get pointer to the "real array" of M
		double *TReal = mxGetPr(M);
		//Pointer to the "imaginary array" of M
		double *TIm = mxGetPi(M);
		//Populate data
		for (unsigned j = 0; j < dimension; j++){
			for (unsigned i = 0; i < intervaln; i++){
				complex<double> num(*(TReal + j*intervaln+i),0);
				//Imaginary can be zero
				if (TIm){
					num.imag(*(TIm + j*intervaln+i));
				}
				data->DFTSet(i,j,num);
			}
		}
		mxDestroyArray(M);			//Delete matrix and free memory
		cout << "Done.\n";
	}
	//FFT
	void MatlabFFT(std::string arg, MatlabData_T &MatlabData){
		//Initialise
		MatlabInitMat(MatlabData);
		cout << "Performing FFT...";
		MatlabData.M->DiscreteFourierTransform();
		cout << "Done\n";
	}
	//IFFT
	void MatlabIFFT(std::string arg, MatlabData_T &MatlabData){
		//Initialise
		MatlabInitMat(MatlabData);
		cout << "Performing Inverse FFT...";
		MatlabData.M->InverseDiscreteFourierTransform();
		cout << "Done\n";
	}

	//Open
	void MatlabOpen(std::string arg, MatlabData_T &MatlabData){
		Matlab.MakeVisible();
	}

	//Close
	void MatlabClose(std::string arg, MatlabData_T &MatlabData){
		Matlab.MakeInvisible();
	}
	//Plot()
	void MatlabPlot(std::string arg, MatlabData_T &MatlabData){
		stringstream cmd = stringstream(arg);
		//Get Dimension
		unsigned int dimension = 0;
		cmd >> dimension;

		if (!dimension || dimension > MatlabData.T ->DFTDimension()){
			cout << "Invalid dimension. Please see help for more information. \n";
			return;
		}
		
		//Create subplot
		engEvalString(Matlab(),"subplot(2,2,1)");
		cout << "Creating graph... (see Matlab Commands below)\n";
		cout << "\t" << "subplot(2,2,1)" << "\n";

		stringstream _dimension;				//Dimension
		_dimension << dimension;

		stringstream _interval;
		_interval << MatlabData.T->DFTInterval();

		stringstream _endTime;
		_endTime << MatlabData.T->DFTInterval()*MatlabData.T->DFTNumInterval()-MatlabData.T->DFTInterval();

		MatlabSend("f FData",MatlabData);
		MatlabSend("t TData", MatlabData);

		string _cmd;

		//Time Domain Real
		_cmd = "DataX = 0:" + _interval.str() + ":" + _endTime.str();			//X-Axis
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "stairs(DataX, real(TData(:," + _dimension.str() + ")))";
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "title('Time Domain Dimension " + _dimension.str() + " (Real)', 'FontWeight', 'bold')";	//Title
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "xlabel('Time in Seconds')";
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		//Time Domain Imaginary
		_cmd = "subplot(2,2,2)";
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "DataX = 0:" + _interval.str() + ":" + _endTime.str();			//X-Axis
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "stairs(DataX, imag(TData(:," + _dimension.str() + ")))";
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "title('Time Domain Dimension " + _dimension.str() + " (Imaginary)', 'FontWeight', 'bold')";	//Title
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "xlabel('Time in Seconds')";
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		//Plot Freq - Real
		_cmd = "subplot(2,2,3)";
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		stringstream _freqInterval;
		_freqInterval << unsigned(MatlabData.F->DFTNumInterval()/2);			//From zero to Nyquist frequency (one-half the sampling rate) i.e. N/2

		_cmd = "FreqX = 1:1:" + _freqInterval.str();			//X-Axis
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "stairs(FreqX, real(FData(1:" + _freqInterval.str() +"," + _dimension.str() + ")))";
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "title('Frequency Domain Dimension " + _dimension.str() + " (Real)', 'FontWeight', 'bold')";	//Title
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "xlabel('Frequency Samples')";
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		//Plot Freq - Imaginary
		_cmd = "subplot(2,2,4)";
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "stairs(FreqX, imag(FData(1:" + _freqInterval.str() +"," + _dimension.str() + ")))";
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "title('Frequency Domain Dimension " + _dimension.str() + " (Imaginary)', 'FontWeight', 'bold')";	//Title
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());

		_cmd = "xlabel('Frequency Samples')";
		cout << "\t" << _cmd << "\n";
		engEvalString(Matlab(),_cmd.c_str());
		
		engEvalString(Matlab(),"figure(gcf)");

		cout << "For more precise control over the graph plot, consider using Matlab itself.\n";
	}

	//Wave
	void MatlabWave(std::string arg, MatlabData_T &MatlabData){
		cout << "Note: Any changes you make in Wave will NOT be reflected in Matlab.\n";
		//Data needed Channel, SampleRate, SampleSize
		
		//Sample Rate
		cout << "Please enter the Sample Rate of the Wave file";
		stringstream _SampleRate = GetInputLine(false, false);
		unsigned SampleRate;
		_SampleRate >> SampleRate;

		//BitRate
		stringstream _BitRate;
		unsigned BitRate = 0;
		do{
			cout << "Please enter the bit rate per sample";
			_BitRate = GetInputLine(false, false);
			_BitRate >> BitRate;
		}while (BitRate && BitRate % 8 != 0);

		cout << "Collapsing T data...\n";
		
		//Channel
		unsigned channels = MatlabData.T->DFTDimension();
		//Build Data
		vector<char> data;
		for (unsigned i = 0; i < MatlabData.T->DFTNumInterval(); i++){
			for (unsigned j = 0; j < channels; j++){
				int num = MatlabData.T->DFTGet(i, j).real();
				Wave::Word bytes = Wave::GetBytesFromInt(num);
				for (unsigned k = 0; k < BitRate/8; k++){
					data.push_back(bytes.GetByte(k));
				}
			}
		}
		cout << "Creating object...";

		try{
			Wave::WaveFile WaveObj = Wave::WaveFile::CreateObject(channels, SampleRate, BitRate, data);
			DFT::DFTGenericFrequency FreqObj;

			PresetWave = &WaveObj;
			PresetFreq = &FreqObj;
			cout << "Launching wave...\n";
			//Launch Wave
			LaunchModule(&WaveMain, "", "wave");
		}
		catch (Exception &e){
			cout << "Error creating wave object. Error: "<< e.GetErrorMessage() << "\n";
			return;
		}
	}
}