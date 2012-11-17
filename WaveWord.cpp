#include "WaveWord.h"
#include "Exception.h"

namespace Wave{

	//Protected methods
	//GetSum()
	unsigned int Word::GetSum() const{
		unsigned int result = 0;
		for (int i = 0 ; i < WORD_SIZE; i++){
			result += (unsigned int)Data[i];
		}
		return result;
	}

	//Constructor
	//Word()
	Word::Word(): Size(0){
		for (int i = 0; i < WORD_SIZE; i++){
			Data[i] = 0x0;
		}
	}
	//With data
	Word::Word(const char *data, unsigned int size):Size(size){
		if (size > WORD_SIZE){
			throw Exception(EXCEPTION_RANGE,"Word Size limit exceeded.");
		}
		unsigned int i;
		for (i = 0; i < size; i++){
			Data[i] = *(data + i);
		}
		for (; i < WORD_SIZE; i++){
			Data[i] = 0x0;
		}
	}
	//Unsigned
	Word::Word(const unsigned char* data , unsigned int size):Size(size){
		//Since casting to signed and unsigned does not change the bit pattern
		if (size > WORD_SIZE){
			throw Exception(EXCEPTION_RANGE,"Word Size limit exceeded.");
		}
		unsigned int i;
		for (i = 0; i < size; i++){
			Data[i] = signed char(*(data + i));
		}
		for (; i < WORD_SIZE; i++){
			Data[i] = 0x0;
		}
	}

	//String overload
	Word::Word(string data){
		if (data.length() > WORD_SIZE){
			throw Exception(EXCEPTION_RANGE,"Word Size limit exceeded.");
		}
		unsigned int i, length = data.length();
		for (i = 0; i < length; i++){
			Data[i] = data[i];
		}
		for (; i < WORD_SIZE; i++){
			Data[i] = 0x0;
		}
	}
	
	//Getters and Setters
	//GetByte()
	char Word::GetByte(unsigned int i) const{
		if (i >= WORD_SIZE){
			throw Exception(EXCEPTION_RANGE,"Word Size limit exceeded.");
		}
		return Data[i];
	}

	//SetByte()
	void Word::SetByte(unsigned int i, char data){
		if (i >= WORD_SIZE){
			throw Exception(EXCEPTION_RANGE,"Word Size limit exceeded.");
		}
		Data[i] = data;
		if (i > Size){
			Size = i;
		}
	}

	//GetSize()
	unsigned int Word::GetSize(){
		return Size;
	}

	//SetSize()
	void Word::SetSize(unsigned int n){
		if (Size > WORD_SIZE){
			throw Exception(EXCEPTION_RANGE,"Word Size limit exceeded.");
		}
		Size = n;
	}

	//GetPointer()
	char *Word::GetPointer(){
		return Data;
	}

	//GetString()
	string Word::GetString(){
		//WARNING: If your bytes contain null characters, they will be terminated accordingly.
		string result;
		for (unsigned int i = 0; i < Size; i++){
			result += Data[i];
		}
		return result;
	}

	//PadBytes()
	void Word::PadBytes(Endianess endian){
		if (Size == WORD_SIZE){
			return;
		}
		if (endian == Little){
			for (unsigned i = Size; i < WORD_SIZE; i++ ){
				Data[i] = 0x0;
			}
		}
		else{
			//Shift data to the back
			unsigned i = Size-1, j = WORD_SIZE-1;
			for (; i >= 0; i--, j--){
				Data[j] = Data[i];
			}
			//Zero pad
			for (; j >=0; j--){
				Data[j] = 0x0;
			}
		}
		Size = WORD_SIZE;
	}
	//IsEmpty()
	bool Word::IsEmpty(){
		return ( Size == 0 );
	}

	//IsAllNull()
	bool Word::IsAllNull(){
		bool result = false;
		for (unsigned int i = 0; i < Size; i++){
			result = result || ( Data[i] == '\0' );
		}
		return result;
	}

	//Overloaded Operator
	//[]
	char& Word::operator[](unsigned int i){
		if (i >= WORD_SIZE){
			throw Exception(EXCEPTION_RANGE,"Word Size limit exceeded.");
		}
		if (i > Size){
			Size = i;
		}
		return Data[i];
	}
	//Const []
	char const& Word::operator[](unsigned int i) const{
		if (i >= WORD_SIZE || i > Size){
			throw Exception(EXCEPTION_RANGE,"Word Size limit exceeded.");
		}
		return Data[i];
	}

	bool Word::operator<(const Word &op) const{
		return ( GetSum() < op.GetSum() );
	}

	//==
	bool Word::operator==(const Word &op) const{
		bool result = true;
		for (int i = 0; i < WORD_SIZE; i++){
			result = result & (Data[i] == op.GetByte(i));
		}
		return result;
	}
	/*
	bool Word::operator==(const char *op) const{
		bool result = true;
		for (int i = 0; i < WORD_SIZE; i++){
			if (*(op+i) == '\0'){
				return false;
			}
			result = result & (Data[i] == * ( op+i ) );
		}
		return result;
	}
	*/
	//!=
	bool Word::operator!=(const Word &op) const{
		return !(operator==(op));
	}

	/*
		Overloaded Functions
	*/
	//GetUnsignedInt()
	unsigned int GetUnsignedInt(Word data, Endianess endian){
		return GetUnsignedInt(data.GetPointer(), data.GetSize(), endian);
	}
	//GetSignedInt()
	int GetSignedInt(Word data, Endianess endian){
		return GetSignedInt(data.GetPointer(), data.GetSize(), endian);
	}	

	//Get Bytes from Int
	Word GetBytesFromInt(int data, Endianess endian){
		//Since conversion to signed and unsigned does not cause any change in the bit pattern, 
		//it doesn't really matter here
		return GetBytesFromUnsigned( (unsigned) data, endian);
	}
	//Unsigned Version
	Word GetBytesFromUnsigned(unsigned int data, Endianess endian){
		//Basic Principle: Treat each byte as a digit of base 256.
		unsigned char bytes[WORD_SIZE];
		unsigned int i = 0;
		while (data != 0){
			bytes[i] = data % 256;
			data /= 256;
			i++;
		}
		//Now i is the length
		if (endian == Little){
			//Data Already in little endian form
			Word TheWord(bytes, i);
			return TheWord;
		}
		else{
			Word TheWord;
			TheWord.SetSize(i);
			i--;
			for (unsigned j = 0; i >= 0; i--, j++){
				TheWord.SetByte(j, bytes[i]);
			}
			return TheWord;
		}
	}
}