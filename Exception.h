/*
	A unified Exception system with the proper error code for better cross module operability

	StackWalker: http://www.codeproject.com/KB/threads/StackWalker.aspx

	RaiseWarning is used to write to the output window if debugging is enabled and the debugger is present
*/
#pragma once
#ifndef Exception_H
#define Exception_H

#define NDEBUG			//Define for non debugging

#include <string>
using namespace std;

/**
	Debugging Specific
	http://msdn.microsoft.com/en-us/library/ms679283(v=VS.85).aspx
**/

#ifndef NDEBUG			//Define the MACRO NDEBUG to prevent this debugging code
#define _WIN32_WINNT 0x0400		//Define the Windows version
#include <windows.h>
#include "StackWalker.h"		//See http://www.codeproject.com/KB/threads/StackWalker.aspx

void RaiseWarning(LPTSTR warning_text);			//Debug version


#else /*NDEBUG*/
void RaiseWarning(char *warning_text);			//Does nothing
void RaiseWarning(wchar_t *warning_text);		//Does nothing

typedef char StackWalker;			//Placeholder
#endif /*NDEBUG*/

/*
	Exception Class
*/
class Exception{
	int ErrorCode;			//Integer with the error code
	string ErrorMessage;	//String of error message
	int ExtraCode;			//Any extra error code specific to the class
	string ExtraMessage;		//Any extra message in String specific to the class
	StackWalker CallStack;	//Call stack whose behaviour depends on the mode;

public:
	/******
	Exception 
	*******/
	Exception(int error_code, const char *error_message = 0, int extra_code = 0, const char *extra_message = 0);
	Exception(int error_code, const string error_message = "", int extra_code = 0, const string extra_message = "");	//String overload

	//Getters
	int GetErrorCode() const;			//Error code
	string GetErrorMessage() const;		//Error string
	int GetExtraCode() const;			//Extra code
	string GetExtraMessage() const;	//Extra Message

	//Get Call stack whose behaviour depends on the mode. If debugging mode is off, StackWalker is simply an zero valued character
	StackWalker &GetCallStack();		
};

/***************
	Global Exception codes for use.

	Classes may define their own set of error codes. These should only be carried in extra code
****************/
const int EXCEPTION_UNKNOWN = 0x1;						//Unknown exception. Should not be used at all actually
const int EXCEPTION_UNEXPECTED = 0x2;					//Unexpected exception.	Should not be used
const int EXCEPTION_INITIALISATION = 0x3;				//Initialisation fail
const int EXCEPTION_UNSUPPORTED = 0x4;					//Unsupported operation

const int EXCEPTION_RANGE = 0x11;						//Out of range exception

const int EXCEPTION_MEMORY_ERROR = 0x101;				//Used in lieu of bad_alloc

const int EXCEPTION_FILE_CANNOT_OPEN = 0x201;				//Generic cannot open file
const int EXCEPTION_FILE_CANNOT_OPEN_INPUT = 0x202;				//Cannot open file for input
const int EXCEPTION_FILE_CANNOT_OPEN_OUTPUT = 0x203;				//Cannot open file for output
const int EXCEPTION_FILE_NOT_OPEN = 0x204;					//File is not open!

const int EXCEPTION_PARSE_ERROR = 0x301;				//Generic parse error
const int EXCEPTION_PARSE_MISSING_TOKEN = 0x302;		//Expected token was missing
const int EXCEPTION_PARSE_FORMAT_ERROR = 0x303;			//Parse encountered a format it did not recognise or support
const int EXCEPTION_PARSE_MISSING_DATA = 0x304;			//Data was supposed to be there but was missing

const int EXCEPTION_DATA_ERROR = 0x401;					//Generic data error
const int EXCEPTION_DATA_FORMAT_ERROR = 0x402;			//Data provided was of the wrong format
const int EXCEPTION_DATA_INVALID = 0x403;				//Invalid data provided

#endif /*Exception_H*/