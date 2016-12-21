#pragma once
#include <exception>

class MyException : public std::exception
{
public:
	MyException(void);
	virtual ~MyException(void);


	virtual const char* name() const throw();
		/// Returns a static string describing the exception.

	virtual const char* what() const throw();
		/// Returns a static string describing the exception.
};
