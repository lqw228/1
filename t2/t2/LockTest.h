#ifndef Foundation_LockTest_INCLUDED
#define Foundation_LockTest_INCLUDED

template <class M>
class LockTest
{
public:
	
	LockTest(M& metex)
	{
		_pMutex = &metex;
		//metex.Abc();
		//metex.Func();
	}

	void Open()
	{
		_pMutex->Abc();
		_pMutex->Func();
	}
private:
	LockTest()
	{
	}
	M* _pMutex;
};
#endif