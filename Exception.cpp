/*
	A unified Exception system with the proper error code for better cross module operability
*/
#include "Exception.h"

/**
	Raise Warning - Debugging Specific
	http://msdn.microsoft.com/en-us/library/ms679283(v=VS.85).aspx
**/

#ifndef NDEBUG			//Define the MACRO NDEBUG to prevent this debugging code

/*
	See the following sites for information on the string types defined by Windows
	- http://msdn.microsoft.com/en-us/library/dd374131(v=vs.85).aspx

	Since most projects will be using unicode, prepend "L" before a string literal to use wchar_t instead of char
	- http://msdn.microsoft.com/en-us/library/c426s321(v=VS.100).aspx
*/
void RaiseWarning(LPTSTR warning_text){
	if (IsDebuggerPresent()){
		OutputDebugString( warning_text );
		OutputDebugString(L"\n");
		//StackWalker sw;
		//sw.ShowCallstack();
	}
}

#else /*NDEBUG*/
void RaiseWarning(char *warning_text){
	return;		//Does nothing
}
void RaiseWarning(wchar_t *warning_text){
	return;		//Does nothing
}

#endif /*NDEBUG*/

/**
	Exception Class
**/
//Constructor
Exception::Exception(int error_code, const char *error_message, int extra_code, const char *extra_message) :ErrorCode(error_code), ExtraCode(extra_code){
	if (error_message){
		ErrorMessage = error_message;
	}
	if (extra_message){
		ExtraMessage = extra_message;
	}
}

Exception::Exception(int error_code, const string error_message, int extra_code, const string extra_message)
	:ErrorCode(error_code), ErrorMessage(error_message), ExtraCode(extra_code), ExtraMessage(extra_message)
	{}

//GetErrorCode()
int Exception::GetErrorCode() const{
	return ErrorCode;
}
//GetErrorMessage()
string Exception::GetErrorMessage() const{
	return ErrorMessage;
}
//GetExtraCode()
int Exception::GetExtraCode() const{
	return ExtraCode;
}
//GetExtraMessage()
string Exception::GetExtraMessage() const{
	return ExtraMessage;
}
//GetCallStack()
StackWalker &Exception::GetCallStack(){
	return CallStack;
}