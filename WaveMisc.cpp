#include "WaveMisc.h"

namespace Wave{
	/*
		Utility Functions
	*/
	//From the string data, reconstruct the signed integer that forms  it. Data is in endian format.
	//Length is in bytes
	//If data contains more than four bytes, the MSB will be truncated.
	//The first bit of the MSB is taken as the sign in the two's complement.
	int GetSignedInt(const char *data, unsigned int length, Endianess endian){
		int result = 0;
		unsigned char MSB;
		bool isNegative;
		if (length == 0){
			return 0;
		}
		if (endian == Big){
			//Do the truncation if length is too long
			if (length > 4){
				data = data+length-4;
				length = 4;
			}
			//Get MSB - MSB is the first byte.
			MSB = *data;
			//isNegative = MSB >> 7;		//Right shift seven bits to see if the Most Significant Bit is one.
			isNegative = MSB > 127;			//Same thing as above. To prevent warning C4800
			if (isNegative){
				MSB -= (char)128;
			}
			for (unsigned int i = length-1; i > 0; i--){
				//result += (int) *(data+i) * pow(256.0, (int) (length - 1 - i) );
				//Left shifting by eight bits = multiplying by 256.
				//We use a two stage casting to prevent the compiler from sign extending when changing from a 
				//2 byte char to a 4 byte int.
				//We first ask the compiler to cast the value to an unsigned char before extending it
				result += ((unsigned int)((unsigned char) *(data+i))) << ((length-1-i)*8);
			}
			//result += (int) MSB * pow(256.0, (int) (length - 1) );
			result += MSB << (length - 1)*8;
		}
		else if (endian == Little){
			if (length > 4){
				length = 4;
			}
			//Get MSB - MSB is last byte
			MSB = *(data+(length-1));
			//isNegative = MSB >> 7;		//Right shift seven bits to see if the Most Significant Bit is one.
			isNegative = MSB > 127;			//Same thing as above. To prevent warning C4800
			if (isNegative){
				MSB -= (char)128;
			}
			for (unsigned int i = 0; i < length-1; i++){
				//Treat each character in data as a number of base-256 and proceed accordingly.
				//Left shifting by eight bits = multiplying by 256.
				//We use a two stage casting to prevent the compiler from sign extending when changing from a 
				//2 byte char to a 4 byte int.
				//We first ask the compiler to cast the value to an unsigned char before extending it
				result += ((unsigned int)((unsigned char) *(data+i))) << (i*8);
				//result += (int) *(data+i) * pow(256.0, (double) i);

			}
			//result += (int) MSB * pow(256.0, (int) (length - 1) );
			result += MSB << (length - 1)*8;
		}
		if (isNegative){
			//result -= pow(2.0, (double) length*8-1 );
			result -= 2U << (length*8-1);
		}
		return result;
	}
	//String overloaded version for convenience sakes
	int GetSignedInt(string data, Endianess endian){
		if (data.empty()){
			return 0;
		}
		unsigned int length = data.length();
		if (length > 4){
			//Truncate
			if (endian == Big){
				data = data.substr(length-4,4);
				length = 4;
			}
			else if (endian == Little){
				data = data.substr(0,4);
				length = 4;
			}
		}
		return GetSignedInt(data.c_str(),length, endian);
	}
	//From the character array data, reconstruct the unsigned integer from the n bytes that from it. Data is in endian format.
	//If data is longer than four bytes, the MSB are truncated
	unsigned int GetUnsignedInt(const char *data, unsigned int length, Endianess endian){
		unsigned int result = 0;
		
		if (length == 0){
			return 0;
		}

		if (endian == Big){
			//Do the truncation if length is too long
			if (length > 4){
				data = data+length-4;
				length = 4;
			}
			for (int i = (signed int)length-1 ; i > -1; i-- ){
				//Treat each character in data as a number of base-256 and proceed accordingly.
				//Left shifting by eight bits = multiplying by 256.
				//We use a two stage casting to prevent the compiler from sign extending when changing from a 
				//2 byte char to a 4 byte int.
				//We first ask the compiler to cast the value to an unsigned char before extending it
				result += ((unsigned int)((unsigned char) *(data+i))) << ((length-1-i)*8);
				//result += (unsigned int) *(data+i) * pow(256.0, (int) (length - 1 - i) );			
			}		
		}
		else if (endian == Little){
			if (length > 4){
				length = 4;
			}
			for (unsigned int i = 0; i < length; i++){
				//Treat each character in data as a number of base-256 and proceed accordingly.
				//Left shifting by eight bits = multiplying by 256.
				//We use a two stage casting to prevent the compiler from sign extending when changing from a 
				//2 byte char to a 4 byte int.
				//We first ask the compiler to cast the value to an unsigned char before extending it
				result += ((unsigned int)((unsigned char) *(data+i))) << (i*8);
			}
		}
		return result;
	}
	//String overloaded version for convenience sakes
	unsigned int GetUnsignedInt(string data, Endianess endian){
		if (data.empty()){
			return 0;
		}
		unsigned int length = data.length();
		if (length > 4){
			//Truncate
			if (endian == Big){
				data = data.substr(length-4,4);
				length = 4;
			}
			else if (endian == Little){
				data = data.substr(0,4);
				length = 4;
			}
		}
		return GetUnsignedInt(data.c_str(),length, endian);
	}
}
