#include <fstream>
#include "Exception.h"
#include "DFTUtility.h"

namespace DFT{
	//Dump to CSV
	void DumpFile(const DFTData *data, const char *fileename, unsigned int buffer_size){
		std::ofstream file(fileename, std::ios_base::out | std::ios_base::trunc);
		if (!file){
			throw Exception(EXCEPTION_FILE_CANNOT_OPEN_OUTPUT, "Unable to open file for writing.");
		}
		char *buffer = NULL;
		try{
			if (buffer_size){
				buffer = new char[buffer_size];
				file.rdbuf()->pubsetbuf(buffer, buffer_size);
			}
		}
		catch(bad_alloc){
			//RaiseWarning(L"Buffer was set too large. Default buffer size used.");
			//Carry on
		}

		//Wrap all the code in one giant try block just in case to prevent memory leaks
		try{
			unsigned intervalN = data->DFTNumInterval();
			unsigned dimension = data->DFTDimension();

			for (unsigned i = 0; i < intervalN; i++){
				for (unsigned j = 0; j < dimension; j++){
					std::complex<double> num = data->DFTGet(i,j);
					file << num.real();
					if (num.imag()){
						if (num.imag() > 0){
							file << '+';
						}
						file << num.imag() << 'i';
					}
					file << ',';
				}
				file << '\n';
			}
		}
		catch(...){
			delete buffer;
			file.close();
			throw;
		}
		delete buffer;
		file.close();
	}
}