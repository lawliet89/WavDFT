#include "WaveFile.h"
#include "Exception.h"
#include <new>

namespace Wave{
	/**
		Protected Methods
	**/
	//GetNextWord()
	Word WaveFile::GetNextWord(){
		if (!File->is_open()){
			throw Exception(EXCEPTION_FILE_NOT_OPEN, "File is not open for processing.");
		}
		char word[4];
		unsigned int size;
		File->read(word,WORD_SIZE);
		size = File->gcount();

		return Word(word, size);
	}
	//GetNextByte()
	char WaveFile::GetNextByte(){
		if (!File->is_open()){
			throw Exception(EXCEPTION_FILE_NOT_OPEN, "File is not open for processing.");
		}
		return char(File->get());
	}
	//GetBytes()
	vector<char> WaveFile::GetBytes(unsigned int n){
		if (!File->is_open()){
			throw Exception(EXCEPTION_FILE_NOT_OPEN, "File is not open for processing.");
		}
		vector<char> data;
		for (unsigned int i = 0; i < n && !File->eof(); i++){
			data.push_back(char(File->get()));
		}
		return data;
	}

	/**
		Public Methods
	**/
	//Destructor
	WaveFile::~WaveFile(){
		if (File->is_open()){
			File->close();
		}
		delete File;
		SubChunks.clear();
		DataSubChunk.Data.clear();
	}

	//Open()
	void WaveFile::Open(const char *file){
		if (File->is_open()){
			File->close();
		}
		File->open(file, ios_base::binary | ios_base::in | ios_base::out);
		if (File->fail()){
			throw Exception(EXCEPTION_FILE_CANNOT_OPEN, "Unable to open Wav File.");
		}
	}

	//Parse()
	unsigned int WaveFile::Parse(){
		if (!File->is_open()){
			throw Exception(EXCEPTION_FILE_NOT_OPEN, "File is not open for processing.");
		}
		File->clear();		//Clear the fail bits
		File->seekg(0,ios_base::beg);

		//Begin Parsing
		SubChunks.clear();
		unsigned int count = 0;			//Number of subchunks

		//RIFF ChunkID
		Word CurrentWord = GetNextWord();
		if (CurrentWord != Word("RIFF",4)){			//First Word should be the words "RIFF"
			throw Exception(EXCEPTION_PARSE_FORMAT_ERROR, "File does not have a proper RIFF header!", WAVE_RIFF_MISSING);
		}
		
		//Read ChunkSize
		CurrentWord = GetNextWord();
		ChunkSize = GetUnsignedInt(CurrentWord);

		//Read ChunkFormat
		CurrentWord = GetNextWord();
		if (CurrentWord != Word("WAVE",4)){			//Should read WAVE
			throw Exception(EXCEPTION_PARSE_FORMAT_ERROR, "File is not Wave!", WAVE_NOT_WAVE);
		}

		//Time to read SubChunks
		//A PCM file should only have two subchunks but this was done to extend compatibility a little.
		//So long as the audio is in PCM, all is good.
		while(!File->eof()){
			//Get the ID
			Word SubChunkID = GetNextWord();
			if(SubChunkID.IsAllNull()){
				//Huh? Empty word.
				continue;
			}
			unsigned int SubChunkSize = GetUnsignedInt(GetNextWord());
			if (SubChunkSize == 0){
				continue;
			}

			//Check for "special chunks"
			if (SubChunkID == "data"){
				//Then let's not load the file into memory
				DataSubChunk.Begin = File->tellg();
				DataSubChunk.Size = SubChunkSize;
				File->seekg(SubChunkSize, ios_base::cur);
				DataSubChunk.End = File->tellg();
				//We do not know if EOF has been reached. It's really bizzare how seekg works. It does not set failbit or eofbit if EOF is reached
				//We use peek() to set the proper flags.
				File->peek();
				if (File->eof()){
					DataSubChunk.IsLastChunk = true;
				}
				else{
					DataSubChunk.IsLastChunk = false;
				}
			}
			else{
				vector<Word> SubChunkData;
				bool SubChunkComplete = true;
				unsigned int i = WORD_SIZE;
				unsigned int mod = SubChunkSize % WORD_SIZE;		//Get the modulus of SubChunkSize and WordSize in case it is not a multiple of WordSize.
				//Read chunk into a list
				while (i <= SubChunkSize && !File->eof()){
					SubChunkData.push_back(GetNextWord());
					i += WORD_SIZE;
				}
				if (File->eof() && i < SubChunkSize){
					//Incomplete Chunk. SubChunkSize was larger than the Actual size
					SubChunkComplete = false;
				}
				if (mod != 0){
					//ChunkSize is not a multiple of WORD_SIZE
					char *Bytes = new char[mod];
					unsigned int j;
					for (j = 0; j < mod && !File->eof(); j++){
						*(Bytes+j) = GetNextByte();
					}
					if ( (j + 1) < mod){
						//Incomplete Chunk. SubChunkSize was larger than the Actual size
						SubChunkComplete = false;
					}
					SubChunkData.push_back(Word(Bytes, j+1));
					delete[] Bytes;
				}

				//Add to SubChunks
				SubChunks[SubChunkID] = WaveChunk<>(SubChunkID,SubChunkSize,SubChunkData,SubChunkComplete);
				
			}
			count++;
		}
		//Now that we have the chunks let's populate some data.
		//See http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
		if (SubChunks.count("fmt ") == 0){			//Reading from 'fmt ' subchunk
			//throw runtime_error("File is missing 'fmt ' subchunk!");
			throw Exception(EXCEPTION_PARSE_MISSING_TOKEN, "File is missing 'fmt ' subchunk!", WAVE_FMT_MISSING);
		}
		vector<Word> fmtData = SubChunks[Word("fmt ")].GetData();
		//Extract AudioFormat and NumChannels
		if (fmtData.size() < 4){
			throw Exception(EXCEPTION_PARSE_MISSING_DATA, "'fmt ' subchunk is missing information", WAVE_FMT_INCOMPLETE);
		}
		CurrentWord = fmtData[0];
		char Nibble[2];			//Well not really a nibble but...

		//Extract AudioFormat
		Nibble[0] = CurrentWord[0];
		Nibble[1] = CurrentWord[1];
		DataSubChunk.FormatCode = GetUnsignedInt(Nibble,2);

		//Audio Format tag
		if (DataSubChunk.FormatCode != WAVE_FORMAT_PCM){
			//Check for extensible
			if (DataSubChunk.FormatCode == WAVE_FORMAT_EXTENSIBLE){
				if (fmtData.size() != 10){
					throw Exception(EXCEPTION_PARSE_MISSING_DATA, "'fmt ' subchunk is missing information", WAVE_FMT_INCOMPLETE);
				}
				DataSubChunk.IsExtended = true;
			}
			else{
				//NOT PCM!
				throw Exception(EXCEPTION_PARSE_FORMAT_ERROR, "File is not in PCM!", WAVE_NOT_PCM);
			}
		}
		else{
			DataSubChunk.IsExtended = false;
		}

		//Extract NumChannel
		Nibble[0] = CurrentWord[2];
		Nibble[1] = CurrentWord[3];
		DataSubChunk.NumChannels = (short) GetUnsignedInt(Nibble,2);

		//Extract SampleRate
		DataSubChunk.SampleRate = GetUnsignedInt(fmtData[1]);

		//Extract ByteRate
		DataSubChunk.ByteRate = GetUnsignedInt(fmtData[2]);

		//Extract BlockSize and Sample Size
		CurrentWord = fmtData[3];
		Nibble[0] = CurrentWord[0];
		Nibble[1] = CurrentWord[1];

		DataSubChunk.BlockSize = (short) GetUnsignedInt(Nibble,2);

		Nibble[0] = CurrentWord[2];
		Nibble[1] = CurrentWord[3];
		DataSubChunk.SampleSize = (short) GetUnsignedInt(Nibble,2);

		if (DataSubChunk.SampleSize > MAX_SAMPLE_SIZE)
			throw Exception(EXCEPTION_PARSE_FORMAT_ERROR, "File Bitrate is too high!", WAVE_BITRATE_HIGH);

		//Read format tag again
		if (DataSubChunk.IsExtended){
			//Read a few more bytes
			//Skip the next two bytes
			CurrentWord = fmtData[6];
			//Our format code is in the first two bytes
			Nibble[0] = CurrentWord[0];
			Nibble[1] = CurrentWord[1];
			DataSubChunk.FormatCode = GetUnsignedInt(Nibble,2);

			if (DataSubChunk.FormatCode != WAVE_FORMAT_PCM){
				throw Exception(EXCEPTION_PARSE_FORMAT_ERROR, "File is not in PCM!", WAVE_NOT_PCM);
			}
		}

		return count;
	}

	//NumBlocks();
	unsigned int WaveFile::NumBlocks() const{
		return (DataSubChunk.BlockSize == 0) ? 0 : (DataSubChunk.Size/DataSubChunk.BlockSize);
	}

	//NumSamples()
	unsigned int WaveFile::NumSamples() const{
		return NumBlocks()*DataSubChunk.NumChannels;
	}

	/****************************
	//Methods to relate to loading data
	*****************************/
	//DataIsLoaded()
	bool WaveFile::DataIsLoaded(){
		DataSubChunk.IsLoaded = !DataSubChunk.Data.empty();
		return DataSubChunk.IsLoaded;
	}
	//DataUnload()
	void WaveFile::DataUnload(){
		if (!File->is_open()){
			throw Exception(EXCEPTION_FILE_NOT_OPEN, "File is not open. Cannot afford to unload data.");
		}
		DataSubChunk.Data.resize(1);		//Resize to return memory
		DataSubChunk.Data.clear();			//Clear data
		DataIsLoaded();						//Set flags
		DataSubChunk.Iterator = DataSubChunk.Data.begin();
	}
	//DataLoad()
	void WaveFile::DataLoad(){
		if (!File->is_open()){
			throw Exception(EXCEPTION_FILE_NOT_OPEN, "File is not open for processing.");
		}
		//Clear any prior data
		DataUnload();
		//Reserve space
		DataSubChunk.Data.reserve(DataSubChunk.Size);
		DataSubChunk.Data.clear();			//After reserving, the vector will be of size 1 and have a zeroth element. We don't want this
		File->clear();
		File->seekg(DataSubChunk.Begin);
		for (unsigned i = 0; i < DataSubChunk.Size && !File->eof(); i++){
			char Byte = GetNextByte();
			DataSubChunk.Data.push_back(Byte);
		}

		//Load iterator
		DataSubChunk.Iterator = DataSubChunk.Data.begin();
	}

	/****************************
	**	"Iterator" Methods for reading Stream Data
	*****************************/
	//DataRewind()
	void WaveFile::DataRewind(){
		if (DataIsLoaded()){
			DataSubChunk.Iterator = DataSubChunk.Data.begin();
		}
		else{
			if (!File->is_open()){
				throw Exception(EXCEPTION_FILE_NOT_OPEN, "File is not open for processing.");
			}
			File->clear();
			File->seekg(DataSubChunk.Begin);
		}
	}
	//DataEnd()
	bool WaveFile::DataEnd(){
		if (DataIsLoaded()){
			return !(DataSubChunk.Iterator < DataSubChunk.Data.end());
		}
		else{
			if (!File->is_open()){
				throw Exception(EXCEPTION_FILE_NOT_OPEN, "File is not open for processing.");
			}
			streampos current = File->tellg();
			if (current >=  DataSubChunk.Begin && current < DataSubChunk.End){
				return false;
			}
		}
		return true;
	}
	//DataNextBlock() - Signed Data
	WaveBlock<int> WaveFile::DataNextBlock(){
		if (DataEnd()){
			//File pointer not in the right place or if EOF has been reached
			return WaveBlock<>();
		}
		//Initialise data
		WaveBlock<int> Block;
		Block.SetChannelsCount(DataSubChunk.NumChannels);
		//Data to process
		vector<char> Data;
		if (DataIsLoaded()){
			for (unsigned i = 0; i < DataSubChunk.BlockSize && DataSubChunk.Iterator < DataSubChunk.Data.end(); i++){
				Data.push_back(*DataSubChunk.Iterator);
				DataSubChunk.Iterator++;
			}
		}
		else{
			if (!File->is_open()){
				throw Exception(EXCEPTION_FILE_NOT_OPEN, "File is not open for processing.");
			}
			 Data = GetBytes(DataSubChunk.BlockSize);
		}
		//Check if the number of bytes actually match what is supposed to be there. Probably because unexpected EOF was reached
		if (Data.size() != DataSubChunk.BlockSize){
			throw Exception(EXCEPTION_PARSE_MISSING_DATA, "Missing bytes in the block being read.", WAVE_DATA_MISSING);
		}
		for (unsigned int i = 0; i < DataSubChunk.NumChannels; i++){
			char Bytes[MAX_SAMPLE_SIZE/8];					//Maximum Sample Size
			for (unsigned int j = 0; j < unsigned(DataSubChunk.SampleSize/8); j++){		//We are assuming a sample size in multiples of 8... Otherwise need a buffer?
				//Note if the sample size is > 8 bits, the data is actually signed. 
				Bytes[j] = Data[i*DataSubChunk.SampleSize/8+j];
			}
			Block.SetChannel(i, GetSignedInt(Bytes,DataSubChunk.SampleSize/8));
		}
		return Block;
	}
	//DataNextBlockUnsigned() - Signed Data
	WaveBlock<unsigned int> WaveFile::DataNextBlockUnsigned(){
		if (DataEnd()){
			//File pointer not in the right place or if EOF has been reached
			return WaveBlock<unsigned int>();
		}
		//Initialise data
		WaveBlock<unsigned int> Block;
		Block.SetChannelsCount(DataSubChunk.NumChannels);
		//Data to process
		vector<char> Data;
		if (DataIsLoaded()){
			for (unsigned i = 0; i < DataSubChunk.BlockSize && DataSubChunk.Iterator < DataSubChunk.Data.end(); i++){
				Data.push_back(*DataSubChunk.Iterator);
				DataSubChunk.Iterator++;
			}
		}
		else{
			if (!File->is_open()){
				throw Exception(EXCEPTION_FILE_NOT_OPEN, "File is not open for processing.");
			}
			 Data = GetBytes(DataSubChunk.BlockSize);
		}
		//Check if the number of bytes actually match what is supposed to be there. Probably because unexpected EOF was reached
		if (Data.size() != DataSubChunk.BlockSize){
			throw Exception(EXCEPTION_PARSE_MISSING_DATA, "Missing bytes in the block being read.", WAVE_DATA_MISSING);
		}
		for (unsigned int i = 0; i < DataSubChunk.NumChannels; i++){
			char Bytes[MAX_SAMPLE_SIZE/8];					//Maximum Sample Size
			for (unsigned int j = 0; j < unsigned(DataSubChunk.SampleSize/8); j++){		//We are assuming a sample size in multiples of 8... Otherwise need a buffer?
				//Note if the sample size is > 8 bits, the data is actually signed. 
				Bytes[j] = Data[i*DataSubChunk.SampleSize/8+j];
			}
			Block.SetChannel(i, GetUnsignedInt(Bytes,DataSubChunk.SampleSize/8));
		}
		return Block;
	}

	//DataEdit() - Signed version
	void WaveFile::DataEdit(unsigned int interval, unsigned int dimension, int data){
		//A simple cast will do...
		return DataEdit(interval, dimension, (unsigned) data);
	}
	//Unsigned version
	void WaveFile::DataEdit(unsigned int interval, unsigned int dimension, unsigned int data){
		//Since we are getting a DFT data request, we MUST load the data into memory
		if (!DataIsLoaded()){
			DataLoad();
		}
		Word TheWord = GetBytesFromUnsigned(data, Little);
		if (TheWord.GetSize() > DataSubChunk.SampleSize/8){
			throw Exception(EXCEPTION_RANGE, "Sample value too large compared to sample size allowed.");
		}
		//unsigned i = interval*DataSubChunk.BlockSize;			//The index of first byte of the block
		//unsigned j = dimension * DataSubChunk.SampleSize/8;	//Index of the byte of the dimension to read in the block
		unsigned offset = (interval*DataSubChunk.BlockSize) + (dimension * DataSubChunk.SampleSize/8);		//See i+j
		unsigned k = 0;	//Index
		for (; k < TheWord.GetSize(); k++){
			DataSubChunk.Data[offset+k] = TheWord[k];
		}
		for (; k < DataSubChunk.SampleSize/8; k++){
			DataSubChunk.Data[offset+k] = 0x0;
		}
		//And we are done
	}
	//Operator()
	int WaveFile::operator()(unsigned int interval, unsigned int dimension){
		//Since we are getting a DFT data request, we MUST load the data into memory
		if (!DataIsLoaded()){
			DataLoad();
		}
		//unsigned i = interval*DataSubChunk.BlockSize;			//The index of first byte of the block
		//unsigned j = dimension * DataSubChunk.SampleSize/8;	//Index of the byte of the dimension to read in the block
		unsigned offset = (interval*DataSubChunk.BlockSize) + (dimension * DataSubChunk.SampleSize/8);		//See i+j

		if ( (offset +  DataSubChunk.SampleSize/8-1) > DataSubChunk.Data.size()){
			throw Exception(EXCEPTION_RANGE,"Out of range access.");
		}

		//Construct the data
		char Bytes[MAX_SAMPLE_SIZE/8];					//Maximum Sample Size
		for (unsigned k = 0; k < (unsigned) DataSubChunk.SampleSize/8; k++){
			Bytes[k] = DataSubChunk.Data[offset+k];
		}
		//Convert
		return GetSignedInt(Bytes,DataSubChunk.SampleSize/8);
	}

	//DataGet()
	int WaveFile::DataGet(unsigned int interval, unsigned int dimension){
		//Alias for the () operator
		return this -> operator()(interval, dimension);
	}

	//Operator[]
	char &WaveFile::operator[](unsigned int n){
		if (n > DataSubChunk.Data.size()){
			throw Exception(EXCEPTION_RANGE,"Out of range access.");
		}
		return DataSubChunk.Data[n];
	}
	char WaveFile::operator[](unsigned int n) const{
		if (n > DataSubChunk.Data.size()){
			throw Exception(EXCEPTION_RANGE,"Out of range access.");
		}
		return DataSubChunk.Data[n];
	}
	//Alias for above
	char WaveFile::DataGetByte(unsigned int n) const{
		return operator[](n);
	}
	//EditDataChunk() - use with care
	//void WaveFile::EditDataChunk(const WaveChunk<vector<char> > &data){
	//	if (data.GetID() != Word("data")){
	//		throw Exception(EXCEPTION_DATA_FORMAT_ERROR, "Data provided is of the wrong subchunk.");
	//	}
	//	DataSubChunk.Data = data.GetData();
	//	DataSubChunk.Size = data.GetSize();
	//}

	/*********************
		Methods inherited from DFT::DFTData
	**********************/
	complex<double> WaveFile::DFTGet(unsigned int interval, unsigned int dimension) const{
		WaveFile *Self = const_cast<WaveFile*>(this);
		return complex<double>(Self->operator()(interval,dimension));
	}

	void WaveFile::DFTSet(unsigned int intervalN, unsigned int dimension, const std::complex<double> &data){
		return DataEdit(intervalN, dimension, (int) data.real());
	}


	/********************
		File operators
	***********************/
	//WriteFile()
	void WaveFile::WriteFile(){
		//Basically just invoke the other overloaded version to run with the object's fstream
		if (!File->is_open()){
			throw Exception(EXCEPTION_FILE_NOT_OPEN, "File is not open!");
		}
		return WriteFile(*File);
	}
	void WaveFile::WriteFile(const char* filename){
		//Invoke the other oveloaded version
		fstream file(filename, ios_base::binary | ios_base::out | ios_base::trunc);
		if (!file){
			throw Exception(EXCEPTION_FILE_CANNOT_OPEN, "Unable to open Wav File.");
		}
		WriteFile(file);
		file.close();
	}
	//The WriteFile() proper
	void WaveFile::WriteFile(fstream &file){
		if (!file){
			throw Exception(EXCEPTION_FILE_CANNOT_OPEN, "Unable to open Wav File.");
		}
		//Load Data
		if (!DataIsLoaded()){
			DataLoad();
		}
		//Clear flags
		file.clear();
		file.seekp(0,ios_base::beg);

		//Write
		file.write("RIFF", 4);		//RIFF Header
		
		//Get chunk size
		unsigned _ChunkSize = DataSubChunk.Data.size() + 36;

		Word ChunkSize = GetBytesFromUnsigned(_ChunkSize);
		ChunkSize.PadBytes();

		file.write(ChunkSize.GetPointer(), WORD_SIZE);

		file.write("WAVE", 4);
		
		//fmt
		file.write("fmt ",4);

		//Chunk Size
		_ChunkSize = 16U;
		ChunkSize = GetBytesFromUnsigned(_ChunkSize);
		ChunkSize.PadBytes();
		file.write(ChunkSize.GetPointer(), WORD_SIZE);

		//Format
		file.put(0x01);			//PCM
		file.put(0x0);
		
		//Channel
		Word Channels = GetBytesFromUnsigned(DataSubChunk.NumChannels);
		file.put(Channels.GetByte(0));
		file.put(Channels.GetByte(1));

		//Sample Rate
		Word SampleRate = GetBytesFromUnsigned(DataSubChunk.SampleRate);
		SampleRate.PadBytes();
		file.write(SampleRate.GetPointer(), WORD_SIZE);

		//ByteRate         == SampleRate * NumChannels * BitsPerSample/8
		Word ByteRate = GetBytesFromUnsigned(DataSubChunk.SampleRate * DataSubChunk.NumChannels * DataSubChunk.SampleSize/8);
		ByteRate.PadBytes();
		file.write(ByteRate.GetPointer(), WORD_SIZE);

		//#4 BlockAlign       == NumChannels * BitsPerSample/8
		Word BlockAlign = GetBytesFromUnsigned(DataSubChunk.NumChannels * DataSubChunk.SampleSize/8);
		file.write(BlockAlign.GetPointer(), 2);

		//Bitrate
		Word bits = GetBytesFromUnsigned(DataSubChunk.SampleSize);
		file.write(bits.GetPointer(), 2);

		//Data Sub chunk
		WaveChunk<vector<char> > _data = CreateDataChunk(DataSubChunk.Data);
		file.write("data", WORD_SIZE);
		vector<char> data = _data.GetData();
		_ChunkSize = data.size();
		ChunkSize = GetBytesFromUnsigned(_ChunkSize);
		ChunkSize.PadBytes();
		file.write(ChunkSize.GetPointer(), WORD_SIZE);

		vector<char>::iterator it2;
		
		for (it2 = data.begin(); it2 < data.end(); it2++){
			file.put(*it2);
		}
		//Done
	}

	/*****************
		Static methods
	*****************/
	//CreatObject - Object Factory
	WaveFile WaveFile::CreateObject(unsigned _channels, unsigned _sampleRate, unsigned _sampleSize, const vector<char> &data){
		WaveFile file;
		//Set Data
		file.DataSubChunk.BlockSize = _channels*_sampleSize/8;
		file.DataSubChunk.ByteRate = _sampleRate*_channels*_sampleSize/8;
		file.DataSubChunk.FormatCode = 1;
		file.DataSubChunk.IsExtended = false;
		file.DataSubChunk.NumChannels = _channels;
		file.DataSubChunk.SampleRate = _sampleRate;
		file.DataSubChunk.SampleSize = _sampleSize;
		file.DataSubChunk.Size = data.size();

		//Populate data
		file.DataSubChunk.Data = data;

		return file;
	}
	//CreatFmtChunk()
	WaveChunk<> WaveFile::CreateFmtChunk(unsigned _channels, unsigned _sampleRate, unsigned _sampleSize){
		WaveChunk<> chunk;
		chunk.Set(Word("fmt ", 4));			//ID
		chunk.Set(16U);						//Size
		chunk.Set(true);					//Is Complete

		//Build data
		vector<Word> Data;
		//#1
		Data.push_back(Word());
		Data[0].SetByte(0,0x0);
		Data[0].SetByte(1,0x1);

		Word channels = GetBytesFromUnsigned(_channels);
		Data[0].SetByte(2, channels.GetByte(0));
		Data[0].SetByte(3, channels.GetByte(1));

		//#2
		Word SampleRate = GetBytesFromUnsigned(_sampleRate);
		SampleRate.PadBytes();
		Data.push_back(SampleRate);

		//#3  ByteRate         == SampleRate * NumChannels * BitsPerSample/8
		Word ByteRate = GetBytesFromUnsigned(_sampleRate * _channels * _sampleSize/8);
		ByteRate.PadBytes();
		Data.push_back(ByteRate);

		//#4 BlockAlign       == NumChannels * BitsPerSample/8
		Data.push_back(GetBytesFromUnsigned(_channels * _sampleSize/8));
		Word bits = GetBytesFromUnsigned(_sampleSize);
		Data[3].SetByte(2, bits.GetByte(0));
		Data[3].SetByte(3, bits.GetByte(1));

		chunk.Set(Data);

		return chunk;
	}
	//CreateDataChunk()
	WaveChunk<vector<char> > WaveFile::CreateDataChunk(const vector<char> &data){
		WaveChunk<vector<char> > chunk;
		chunk.Set(Word("data", 4));				//ID
		chunk.Set(unsigned(data.size()));		//Size
		chunk.Set(true);						//Is Complete
		chunk.Set(data);

		return chunk;
	}
}