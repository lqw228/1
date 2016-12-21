#include "MyException.h"

MyException::MyException(void)
{
}

MyException::~MyException(void)
{
}

const char* MyException::name() const throw()
{
	return "自定义错误模式";
}

const char* MyException::what() const throw()
{
	return name();
}

