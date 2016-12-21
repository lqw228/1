#pragma once

namespace Lqw {

template <class C>
class RunnableAdapter : public Runnable
{
public:
	typedef void (C::*Callback)();

	RunnableAdapter(C& object, Callback method): _pObject(&object), _method(method){};
	virtual ~RunnableAdapter(void){};

	void run()
	{
		(_pObject->*_method)();
	}

private:
	RunnableAdapter(void);

	C*       _pObject;
	Callback _method;
};

}
