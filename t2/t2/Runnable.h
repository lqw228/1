#pragma once
namespace Poco {

class Runnable
{
public:
	Runnable(void);
	virtual ~Runnable(void);

	virtual void run() = 0;
};

}


