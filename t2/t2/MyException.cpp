#include "MyException.h"

MyException::MyException(void)
{
}

MyException::~MyException(void)
{
}

const char* MyException::name() const throw()
{
	return "�Զ������ģʽ";
}

const char* MyException::what() const throw()
{
	return name();
}

