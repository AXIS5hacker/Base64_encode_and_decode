#ifndef L_EXCEPTION_H
#define L_EXCEPTION_H

#include <string>
#include <exception>
using std::exception;
using std::string;

class InvalidLengthException : public exception
{
public:
	InvalidLengthException() {
		//msg = "Invalid Bitstream Length, should be a multiple of 6.";
	}
	const char* what() const
	{
		return msg;
	}
private:
	static const char* const msg;
};

#endif