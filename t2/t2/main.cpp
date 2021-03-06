#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <assert.h>
#include <exception>
#include <stdexcept>
#include <sstream>
#include<queue>
#include <list>

#include "LockTest.h"
#include "Mutex.h"
#include "AtomicCounter.h"
#include "SharedPtr.h"
#include "Nullable.h"
#include "MyException.h"

#include "dynamic_huge_array.h"
#include "queue.h"

using std::cout;
using std::endl;
using namespace std;


//
////using namespace Poco;

//using Poco::Mutex;
//using Poco::AtomicCounter;
//using Poco::SharedPtr;
//using Poco::Nullable;
//using Poco::NullType;
//
//struct stData
//{
//	string Name;
//	int len;
//};
//map<int ,string>maplive[100];
//map<int, string>maplive2;
//vector<string> vecLive[100];
//vector<string> vecLive2;
//vector<string> vecLive3;
//map<string, string> mapValue1;
//map<string, string> mapValue2;
//map<string, string>::iterator iter2;

//map<string, stData> mapValue3;
//map<string, stData>::iterator iter3;

//vector<pair<string, int> > vecQue;

//map<string, int>NameToIndex;
//map<string, int>::iterator iter;
//std::map<string, std::map<int, string > > m_mapFunction2TableName;
//std::map<string, std::map<int, string > >::iterator iter;
//
//
//void func(string strName, int nFuncId, string strTableName)
//{
//
//}
//string Batch
//struct stTable
//{
//	string strBatchName;
//	string strTableName;
//	int nFuncId;
//};
//queue<int> q; //声明队列

//#include "staticF.h"
//#include "MemPool.h"

vector<void*  > vecContent;

#define DATA_BLOCK_LEN 1500

//模拟锁实现
class CRM3TempMutex
{
public:
	void Acquire(){}
	void Release(){}
};

///接收端数据
typedef struct
{
	///指针
	void *	    lpBuffer;
	///长度
	int		dLength;

} RM3_RECEIVER_BIZ_DATA;


typedef CRM3DynamicHugeArray<RM3_RECEIVER_BIZ_DATA, 20>		CReceiverDataHugeArray;

typedef CCircularQueue<CReceiverDataHugeArray*, uint32, CRM3TempMutex> CReceiverDataArrays;

typedef CCircularQueue<int, uint32, CRM3TempMutex> CircularQueueData;


std::map<int,int> m_mapRecvID2Index;

typedef struct stData
{
	int n;
	int w;
	char szBuf[256];
}DATA1;

map<int, DATA1> m_map1;
vector<DATA1*> m_vec1;
map<int , int> map2;
map<int , int>::iterator iter;

typedef struct head
{
	uint32 nLength;
	uint32 nIndex;
}HEAD, *PHEAD;

typedef struct head2
{
	uint32 nLength;
	uint32 nIndex;
	uint32 nCommno;
}HEAD2, *PHEAD2;

struct tagESBHead
{
	uint32 m_iPackType:6;
	uint32 m_iTimeStampFlag:4;
	uint32 m_iMsgOffset:22;

	uint32 m_iPackLen;
	uint32 m_iFuncID;
	uint32 m_iPackID;
	uint32 m_iReturnCode;
	uint32 m_iSenderID;
	uint32 m_iPriority;
	uint32 m_iRmGroupNO;
	uint32 m_iRmNO;
	uint32 m_iQueueNO;
	uint32 m_iQueueTime;
};

//日志服务的业务头
typedef struct tgTradeBizHead 
{
	uint32         dwBufLen;            //长度
	uint32         dwIndex;
	uint32         dwCommNo;            //通信阶段号
}TradeBizHead;

#define BIZ_HEAD_LEN   sizeof(TradeBizHead)

//size_t acd_get_file_size(const char *filename)  
//{  
//	struct stat buf;  
//	if(stat(filename, &buf) < 0)  
//	{  
//		return 0;  
//	}  
//	return (size_t)buf.st_size;  
//} 

int GetFileLen(FILE* m_lpReadFile)
{
	int len = 0;

	//获取文件大小
	fseek(m_lpReadFile, 0L, SEEK_END);
	len = ftell(m_lpReadFile)+1;
	fseek(m_lpReadFile, 0L, SEEK_SET);
	return len;
}

int mf_loadFromFile(const char * lpDebugFileName) 
{
	FILE * lpFile = fopen(lpDebugFileName, "rb");
	/* 文件不存在，直接报错 */
	if (lpFile == NULL)
	{
		//sprintf(szErrorInfo, "no such file=%s", lpDebugFileName);
		return -1;
	}
	size_t iFileLen = GetFileLen(lpFile);

	/* 如果共享内存的大小小于文件大小, 则需要改配置 */
	//size_t iFileLen = acd_get_file_size(lpDebugFileName);
	size_t iShmMB = 1000000000; 
	if ( iShmMB <= iFileLen)
	{
		//sprintf(szErrorInfo, "too large file=%s", lpDebugFileName);
		fclose(lpFile);
		return -2;		
	}

	/* 文件长度为0，则不解析 */
	if (iFileLen <= 0)
	{
		fclose(lpFile);
		return 0;
	}

	/* 申请读取文件的空间 */
	uint32 iBufSize = 1024 * 1024 * 16;
	char * lpReadBuf = (char*)malloc(iBufSize);
	if ( lpReadBuf == NULL )
	{
		//sprintf(szErrorInfo, "no free memory, malloc failed!");
		fclose(lpFile);
		return -3;
	}

	int iReturn = 0;
	size_t iFilePos = 0;
	do
	{
		/* 读取包头 */
		int iReadLen = fread(lpReadBuf, 1, BIZ_HEAD_LEN, lpFile);
		if (iReadLen < (int)BIZ_HEAD_LEN)
		{
			break;
		}

		iFilePos += BIZ_HEAD_LEN;

		TradeBizHead * lpHead = (TradeBizHead*)lpReadBuf;
		uint32 dwBizPackLen = lpHead->dwBufLen;



		/* 读取包体，实际长度=包头+包体+strlen("\r\n") */
		iReadLen = fread(lpReadBuf + sizeof(TradeBizHead), 1, dwBizPackLen + 2, lpFile);
		if (iReadLen < dwBizPackLen + 2 || memcmp(lpReadBuf + sizeof(TradeBizHead) + dwBizPackLen, "\r\n", 2) != 0)
		{
			//sprintf(szErrorInfo, "file=%s is corrupted!", lpDebugFileName);
			iReturn = -6;
			break;
		}	
		iFilePos += iReadLen;


	}while(!feof(lpFile));

	if (lpReadBuf != NULL)
	{
		free(lpReadBuf);
	}

	fclose(lpFile);	

	return iReturn;
}

int ChangeFile()
{
	//写文件模块
	FILE* fp = fopen("d:\\data.dat", "wb");
	//----读文件模块----
	FILE* m_lpReadFile = fopen("d:\\TradeLogSvr_hsrm.trade.uft_m_20161214_114731.dat", "rb");
	int n = 0;
	PHEAD pHd = NULL;
	HEAD hd;
	int ch;
	int nCount;
	int num = 0;

	//获取文件大小
	fseek(m_lpReadFile, 0L, SEEK_END);
	int len =ftell(m_lpReadFile)+1;
	fseek(m_lpReadFile, 0L, SEEK_SET);


	while (1)
	{
		nCount = ftell(m_lpReadFile);
		if (nCount > len - 2)
		{
			break;
		}

		//读头
		fread(&hd, 1, sizeof(hd), m_lpReadFile);
		char* pData = new char[hd.nLength + 20];
		char* pData1 = new char[hd.nLength + 20];
		memset(pData, 0, hd.nLength + 20);
		memset(pData1, 0, hd.nLength + 20);

		//构造新头
		HEAD2 hd2;
		hd2.nLength = hd.nLength + 20;
		hd2.nIndex = hd.nIndex;
		hd2.nCommno = 0;
		fwrite(&hd2, sizeof(HEAD2), 1, fp);


		//读文件内容
		fread((void*)pData, 1, hd.nLength, m_lpReadFile);
		tagESBHead* pHead = (tagESBHead* )pData;
		//printf("num = %, m_iFuncID = %d, m_iPackLen = %d\n",  ++num, pHead->m_iFuncID, pHead->m_iPackLen);
		//构造新内容
		int nHeadLen = sizeof(tagESBHead);
		int nLength = hd.nLength;
		memcpy(pData1, pData, nHeadLen);
		tagESBHead* pHead1 = (tagESBHead* )pData1;
		pHead1->m_iMsgOffset += 20; 
		pHead1->m_iPackLen += 20;
		memcpy(pData1+nHeadLen+20, pData+nHeadLen, nLength-nHeadLen);
		fwrite(pData1, nLength+20, 1, fp);

		//跳过"\r\n"
		ch = fgetc(m_lpReadFile);
		ch = fgetc(m_lpReadFile);
		//写结束符
		char ch2[3]="\r\n";
		fwrite(ch2, 2, 1, fp);

		if (pData)
		{
			delete[] pData;
		}

		if (pData1)
		{
			delete[] pData1;
		}
	}

	fclose(m_lpReadFile);
	fclose(fp);
	return 0;

}

int main()
{
	const char * lpDebugFileName = "d:\\data.dat";
	ChangeFile();
	mf_loadFromFile(lpDebugFileName);
	getchar();
	return 0;
}

/*	int num = 1;
	int i = 0;
	map2[1] = 1*2;
	map2[2] = 2*2;
	map2[5] = 5*2;

	iter = map2.find(2);
	if (iter != map2.end())
	{
		map2.erase(iter);
	}*/	

	//map<int, int>::iterator iter = map2.find(2);
	//if (iter != map.end())
	//{
	//	map2.erase(iter);
	//}

	//for (iter = map2.begin(); iter != map2.end(); iter++)
	//{
	//	cout << iter->second << endl;
	//}


	//DATA1* pData = new DATA1;
	//vector<DATA1*>::iterator it_pos;
	//pData->n = 12;
	//m_vec1.push_back(pData);
	//pData = new DATA1;
	//pData->n = 13;
	//m_vec1.push_back(pData);

	//pData = new DATA1;
	//pData->n = 12;
	//m_vec1.push_back(pData);

	//pData = new DATA1;
	//pData->n = 14;
	//m_vec1.push_back(pData);

	//for (i = 0; i < m_vec1.size(); i++)
	//{
	//	printf("%d\n", m_vec1[i]->n);
	//}

	//for (it_pos = m_vec1.begin(); it_pos != m_vec1.end(); )
	//{
	//	DATA1* pTmp = *it_pos;
	//	if (12 == pTmp->n)
	//	{
	//		if (pTmp)
	//		{
	//			delete pTmp;
	//			pTmp = NULL;
	//		}
	//		it_pos = m_vec1.erase(it_pos);
	//	}
	//	else
	//	{
	//		it_pos++;
	//	}
	//}

	//for (i = 0; i < m_vec1.size(); i++)
	//{
	//	printf("---------%d\n", m_vec1[i]->n);
	//}


	////@todo 已下载文件过滤
	//for (it_pos = m_vec1.begin(); it_pos != m_vec1.end(); ) 
	//{
	//	DATA1* strTmp = *it_pos;  
	//	//string strTmp = *it_pos;
	//	if(12 == strTmp->n)
	//	{ //判断是否已下载过, 已下载则从列表删除
	//		//g_Log << TIME << "file:[" << *it_pos << "] found "<< END; //
	//		//it_pos = vecFiles.erase(it_pos);
	//		it_pos = m_vec1.erase(it_pos);

	//	}
	//	else
	//	{
	//		it_pos++;
	//	}		
	//}

	//DATA1 d1;
	//map<int,DATA1>::iterator iter = m_map1.find(9);
	//if ( iter == m_map1.end( ) ) //没找到
	//	cout << "No" << endl;
	//else //找到
	//	cout << "Yes" << endl;

	//m_map1[1] = d1;
	//m_map1[9] = d1;

	// iter = m_map1.find(9);
	//if ( iter == m_map1.end( ) ) //没找到
	//	cout << "No" << endl;
	//else //找到
	//	cout << "Yes" << endl;
	
    
	////序号文件头
	//typedef struct tgRM3IndexFileHead
	//{
	//	///上次通信阶段号
	//	int dwCommNo;	
	//	///上次序号
	//	int dwIndex;
	//	char szTopic[256];

	//}RM3IndexFileHead;

	//int i = 0;

	//map<int , int> mapTmp;
	//for (int i = 0; i< 50; i++)
	//{
	//	mapTmp[i] = i+1;
	//}

	//for (map<int, int>::iterator iter = mapTmp.begin(); iter != mapTmp.end(); )
	//{
	//	
	//	if (18 == iter->first)
	//	{
	//		mapTmp.erase(iter++);
	//		continue;			
	//	}
	//	cout << "first: "<< iter->first<<", second: " << iter->second << endl;
	//	iter++;
	//}

	////---------写文件
	//FILE* m_lpWriteFile;
	//m_lpWriteFile = fopen("abc.dat", "wb");

	//fseek(m_lpWriteFile, 0, SEEK_SET);
	//int num = 3;
	//fwrite(&num, 1, sizeof(num), m_lpWriteFile);

	//for (i = 0; i < num ; i++)
	//{
	//	RM3IndexFileHead head;
	//	head.dwCommNo = i;
	//	head.dwIndex = i * 2;
	//	strcpy(head.szTopic, "aaabbb");
	//	fwrite(&head, 1, sizeof(head), m_lpWriteFile);
	//}
	//fclose(m_lpWriteFile);

	////----读文件模块----
	//FILE* m_lpReadFile = fopen("abc.dat", "rb");
 //   int n = 0;
	//fread(&n, 1, sizeof(n), m_lpReadFile);

	//for (i = 0; i < n; i++)
	//{
	//	RM3IndexFileHead head;
	//	fread(&head, 1, sizeof(head), m_lpReadFile);
	//	cout << head.dwCommNo << " , " << head.dwIndex << ", " << head.szTopic << endl;
	//}

	//fclose(m_lpReadFile);

//vector 查找
//if(find(v.begin(), v.end(), val) != v.end()){
//	    //找到
//}else{
//	    //没找到
//}

// 	void *p = NULL;
// 	int Count = 0;
// 
// 	while(1)
// 	{
// 		p = malloc(1024*1024);
// 		if (p)
// 		{
// 			Count++;
// 			printf("-------count = %d--------\n", Count);
// 		}
// 		else
// 		{
// 			break;
// 		}
// 	}

//int index = 0;
//index = m_mapRecvID2Index[615718];
//index = m_mapRecvID2Index[615718];

//int nTmp = 0;
//int nRet = 0;
//CircularQueueData t1(2, 10, 
//	1, true, false);
////t1.Resize(2);
//for (int i = 0; i < 10; i++)
//{
//	t1.Push(1+i*2);
//}

//for (int i = 0; i < 10; i++)
//{
//	nTmp = t1.Get(i);
//	cout <<"nTmp = "<< nTmp << endl;
//	
//}

//nTmp =  t1.Pop();
//cout <<"nRet1 = "<< nTmp << endl;

//nTmp =  t1.Pop();
//cout <<"nRet2 = "<< nTmp << endl;

//t1.Push(99);
//for (int i = 0; i < 10; i++)
//{
//	nTmp = t1.Get(i);
//	cout <<"nTmp = "<< nTmp << endl;
//}



//for (int i = 0; i < 10; i++)
//{
//	//nTmp = t1.Get(i);
//	//nRet =t1.Pop();
//	//cout <<"nTmp = "<< nTmp << "Ret = " << nRet << endl;		
//}

//for (int i = 10; i < 12; i++)
//{
//	nTmp = t1.Get(i);
//	cout << nTmp << endl;
//}



//CReceiverDataArrays data;
//CReceiverDataHugeArray* p1 = new CReceiverDataHugeArray;
//data.Push(p1);

// 	int nLen = 0;
// 	int i = 0;
// 
// 	typedef CRM3DynamicHugeArray<RM3_RECEIVER_BIZ_DATA, 20>		CSenderDataHugeArray;
// 
// 	CSenderDataHugeArray t1;
// 	for (i = 0; i < 4*1024*1024; i++)
// 	{
// 		RM3_RECEIVER_BIZ_DATA* data = new RM3_RECEIVER_BIZ_DATA;
// 		data->dLength = i+1;
// 		t1.Add(*data);
// 	}
// 
// 	int nCount = t1.GetCount();
// 	int nIndex = t1.GetMaxIndex();
// 
// 	for (i = 0; i < 4; i++)
// 	{
// 		RM3_RECEIVER_BIZ_DATA *p = t1.Get(i);
// 		 nLen = p->dLength;
// 		 cout << nLen << endl;
// 	}
// 
// 	RM3_RECEIVER_BIZ_DATA data1;
// 	data1.dLength = 99;
// 
// 	t1.Set(1, data1);
// 
// 	RM3_RECEIVER_BIZ_DATA *p = t1.Get(1);
// 	nLen = p->dLength;
// 	cout << nLen << endl;

//int n = 1;
//vector<void*> Vec1;	
//vector<void*>::iterator iter = Vec1.begin();//Vec1.end()
//
//if (iter == Vec1.end())
//{
//	n = 2;		
//}
//else
//{
//	n = 3;
//}

//int num = 1 << 16;
//int i = 0;
//char* p = NULL;
//int* pn = (int*)malloc(5*sizeof(int));
//printf("malloc%p\n",pn);
//for(i=0;i<5;i++)
//	pn[i]=i;
//pn = (int*)realloc(pn, 10*sizeof(int));
//printf("realloc%p\n",pn);
//for(i=5;i<10;i++)
//	pn[i]=i;
//for(i=0;i<10;i++)
//	printf("%3d",pn[i]);
//free(pn);

//p=(char *)malloc(100); 
//if(p) 
//	printf("Memory Allocated at: %x",p); 
//else 
//	printf("Not Enough Memory!/n"); 
//getchar(); 
//p=(char *)realloc(p,256); 
//if(p) 
//	printf("Memory Reallocated at: %x",p); 
//else 
//	printf("Not Enough Memory!/n");

//printf("%s\n", "3.0.1.2" __DATE__" "__TIME__);

//GetInteger();
//
//CMemPool myPool1(DATA_BLOCK_LEN, 0, 10);

//cout<<"myPool1 block size = "<<myPool1.BlockSize()<<endl;
//cout<<"myPool1 allocated block num = "<<myPool1.Allocated()<<endl;
//cout<<"myPool1 available block num = "<<myPool1.Available()<<endl<<endl;

//std::vector<void*> ptrs;
//for (int i = 0; i < 10; ++i)
//{
//	ptrs.push_back(myPool1.Get());
//}

//myPool1.Get();

//int iavilable = 0;
//for (std::vector<void*>::iterator it = ptrs.begin(); it != ptrs.end(); ++it)
//{
//	myPool1.Release(*it);
//	++iavilable;
//	cout<<"myPool1 available block num = "<<myPool1.Available()<<endl;
//}

//CMemPool myPool2(DATA_BLOCK_LEN, 5, 10);
//cout<<endl<<"myPool2 block size = "<<myPool2.BlockSize()<<endl;
//cout<<"myPool2 allocated block num = "<<myPool2.Allocated()<<endl;
//cout<<"myPool2 available block num = "<<myPool2.Available()<<endl;


//class wrap
//{
//public:
//	wrap(int llVal)
//	{
//		sprintf(m_szBuf, "%"PRId64, llVal);
//	}
//	char m_szBuf[1024];
//};
//vector<int> L;
//L.push_back( 1 );
//L.push_back( 2 );
//L.push_back( 3 );
//L.push_back( 4 );
//L.push_back( 5 );

//vector<int>::iterator result = find( L.begin( ), L.end( ), 6 ); //查找3
//if ( result == L.end( ) ) //没找到
//	cout << "No" << endl;
//else //找到
//	cout << "Yes" << endl;

//wrap(123);


//--------------排序-----------------
////记录功能号等级
//typedef struct tagFuncLevel
//{
//	int nFuncID;
//	int nLevel;
//
//}FUNC_LEVEL;
//
//list<int> mylist;
//list<int>::iterator it;
//
//list<FUNC_LEVEL> mylist2;
//list<FUNC_LEVEL>::iterator it2;
//
////队列从小到大的插入
//void InsertOrder(int n)
//{
//	bool bInsert = false;
//	for(std::list<int>::iterator it1 = mylist.begin(); it1 != mylist.end(); it1++)
//	{
//		if (*it1 > n)
//		{
//			bInsert = true;
//			mylist.insert(it1, n);
//			break;
//		}		
//	}
//
//	if (!bInsert)
//	{//没有找到，从最后位置插入
//		mylist.push_back(n);
//	}
//}
//
////队列从小到大的插入
//void InsertOrder2(int nFunc, int nLevel)
//{
//	bool bInsert = false;
//	FUNC_LEVEL funcLevel;
//	funcLevel.nFuncID = nFunc;
//	funcLevel.nLevel = nLevel;
//
//	for(std::list<FUNC_LEVEL>::iterator it1 = mylist2.begin(); it1 != mylist2.end(); it1++)
//	{
//		if (it1->nLevel > nLevel)
//		{
//			bInsert = true;
//			mylist2.insert(it1, funcLevel);
//			break;
//		}		
//	}
//
//	if (!bInsert)
//	{//没有找到，从最后位置插入
//		mylist2.push_back(funcLevel);
//	}
//}

//---------------插入顺序测试
//InsertOrder2(1,2);
//InsertOrder2(2,4);
//InsertOrder2(3,2);
//InsertOrder2(4,-1);
//InsertOrder2(5,9);

////几个队列数据
//int n = 5;
////cin>>n;
//for(int i=0;i<n;i++)
//q.push(i+1);
//while(!q.empty())
//{
//	cout<<q.front()<<" , ";
//	q.pop();
//	//if(!q.empty()) //此处需要判断此时队列是否为空
//	//{
//	//	q.push(q.front());
//	//	q.pop();
//	//}
//}
//int nsize = q.size();
//cout<<endl;



//string strTmp;
//m_mapFunction2TableName["a"][12345] = "abc.cvs";
//strTmp = m_mapFunction2TableName["a"][12345];
//
//
////if (iter == m_mapFunction2TableName.end())
////{
////	printf("end");
////}
//if (strTmp == "")
//{
//	printf("end");
//}
//else
//{
//	printf("end 2");
//}


//const char REPLACESYMBOL=24;
//char a = ',';
//printf("%d", a);
//printf("%c", REPLACESYMBOL);

////判断是否存在过
//int n = atoi("abc");
//NameToIndex["b"] = 1;
//NameToIndex["c"] = 1;
//
////NameToIndex.find("a");
//
//iter = NameToIndex.find("b");
//
//if (iter != NameToIndex.end())
//{//find
//	cout << "find" << endl;
//}
//else
//{//no find
//	cout << "no find" << endl;
//}


//class A
//{
//public:
//	static int i;
//public:
//	A ()
//	{
//		i++;
//	}
//	~A()
//	{
//		i--;
//	}
//
//	void fun()
//	{
//		printf("i = %d\n", i);
//	}
//
//
//};
//int A::i = 0;
//
//void funTmp(A a1)
//{
//	static A aaa;
//	a1.fun();
//}
//
//A a1;
//funTmp(a1);


//pair <string,double> product1 ("tomatoes",3.25);
//pair <string,double> product2;
//pair <string,double> product3;
//product2.first = "lightbulbs";     // type of first is string
//product2.second = 0.99;            // type of second is double
//product3 = make_pair ("shoes",20.0);
//cout << "The price of " << product1.first << " is $" << product1.second << "\n";
//cout << "The price of " << product2.first << " is $" << product2.second << "\n";
//cout << "The price of " << product3.first << " is $" << product3.second << "\n";

//pair<string, int> data = make_pair("b", 1);

//vecQue.push_back(data);
//vecQue.push_back(make_pair("a", 1));

//for (int i = 0; i < vecQue.size(); i++)
//{
//	cout << "The price of " << vecQue[i].first << " is $" << vecQue[i].second << "\n";
//	
//}

//解析map存在排序问题
//std::map<string, int> m_mapBatchName;
//m_mapBatchName["b"] = 1;
//m_mapBatchName["a"] = 1;
//std::map<string, int>::iterator iter = m_mapBatchName.begin();
//if (iter != m_mapBatchName.end())
//{
//	cout << iter->first << endl;
//}

//class XmlFileInfo
//{
//
//};
//
////系统抽象接口
//class IXSystem
//{
//public:
//	//初始化
//	virtual bool Init()     = 0;
//	//卸载
//	virtual bool UnInit()   = 0;
//	//加载配置信息
//	virtual bool Load()     = 0;
//	//下载文件
//    virtual bool DownFile() = 0;
//	//发送文件内容
//	virtual bool SendFile() = 0;
//};
//
//class IXLoadFile
//{
//public:
//	virtual bool Init() = 0;
//	virtual bool LoadFile() = 0;
//};


//
//class CSystem : public IXSystem
//{
//public:
//	//初始化
//	virtual bool Init();
//	//卸载
//	virtual bool UnInit();
//	//加载配置信息
//	virtual bool Load();
//	//下载文件
//	virtual bool DownFile();
//	//发送文件内容
//	virtual bool SendFile();
//};

//#define ALIGN_N(x,N)  ((~((N)-1)) & ((x)+((N)-1)))
//
////调整4字节对齐
//#define ALIGN_4(x) ALIGN_N((x),4)
//
//
//struct tagESBHead
//{
//	int m_iPackType:6;
//	int m_iTimeStampFlag:4;
//	int m_iMsgOffset:22;
//
//	int m_iPackLen;
//	int m_iFuncID;
//	int m_iPackID;
//	int m_iReturnCode;
//	int m_iSenderID;
//	int m_iPriority;
//	int m_iRmGroupNO;
//	int m_iRmNO;
//	int m_iQueueNO;
//	int m_iQueueTime;
//	//uint32 m_iQueueDate;//新增日期
//};

//int a = 1;
//printf("%d , %d\n", a, a++);

//std::map<string, std::map<string, int>> m_mapCommonReqNodeNew; 
//m_mapCommonReqNodeNew["name1"]["sub1"] = 1;
//m_mapCommonReqNodeNew["name2"]["sub"] = 1;
//m_mapCommonReqNodeNew["name2"]["sub2"] = 2;
//m_mapCommonReqNodeNew["name2"]["sub3"] = 3;
//
//std::map<string, std::map<string, int> >::iterator map_it = m_mapCommonReqNodeNew.begin();
//
//for (; map_it != m_mapCommonReqNodeNew.end(); map_it++)
//{
//	cout << map_it->first << "\t";
//	std::map<string, int>::iterator iter = map_it->second.begin();
//	for (; iter != map_it->second.end(); iter++)
//	{
//		cout << iter->first << " " << iter->second << " ";
//		//cout  << *iter->first << ", " << *iter->second << " ";
//	}
//	cout  << endl;
//}

//cout << sizeof(struct tagESBHead) << endl;
//cout << sizeof(struct tagESBHead) << endl;
//cout << ALIGN_4(1) << endl;
//cout << ALIGN_4(3) << endl;
//cout << ALIGN_4(4) << endl;
//cout << ALIGN_4(5) << endl;



//void T222()
//{
//	//map 套vector
//	std::map<string, std::vector<int > > m_vFunctionIDsNew;
//	m_vFunctionIDsNew["a"].push_back(1);
//	m_vFunctionIDsNew["a"].push_back(2);
//	m_vFunctionIDsNew["b"].push_back(4);
//	m_vFunctionIDsNew["c"].push_back(5);
//	std::map<string, std::vector<int > >::iterator map_it = m_vFunctionIDsNew.begin();
//
//	for (; map_it != m_vFunctionIDsNew.end(); map_it++)
//	{
//		cout << map_it->first << "\t";
//		std::vector<int >::iterator iter = map_it->second.begin();
//		for (; iter != map_it->second.end(); iter++)
//		{
//			cout  << *iter << ", ";
//		}
//		cout  << endl;
//	}
//}


// 	printf( "---------%s, %d----- new CMachine---------\n" , __FUNCTION__, __LINE__);
// 	maplive2[1] = "a";
// 	maplive2[2] = "a";
// 	maplive2[3] = "a";
// 	cout << maplive2.size() << endl;
// 
// 
// 	maplive2[4] = "a";
// 	maplive2[5] = "a";
// 	maplive2[6] = "a";
// 	cout << maplive2.size() << endl;

//char* p = NULL;
//p = (char*)0x111223;
//printf("%s\n", p);

//-------------内存泄漏问题--------------
// 	char s[10]; 
// 	int* a = new int(10); 
// 	cout << *a << endl;
// 	sprintf(s, "12345678901"); 
// 	cout << *a << endl;
// 	delete a; // a 被前面的给覆盖了

////-------for_each实用---------------
//void myfunction (int i) {  // function:
//	std::cout << ' ' << i;
//}
//
//struct myclass {           // function object type:
//	void operator() (int i) {std::cout << ' ' << i;}
//} myobject;
//
//vector<int> vec;
////cout << vec.size() << endl;
//vec.push_back(1);
//vec.push_back(2);
//vec.push_back(3);
//std::cout << "myvector contains:";
//for_each (vec.begin(), vec.end(), myfunction);
//std::cout << '\n';
//
//std::cout << "myvector contains:";
//for_each (vec.begin(), vec.end(), myobject);
//std::cout << '\n';

//string strBatch = "";
//int nTemp = -1;
//iter = NameToIndex.find(strBatch.c_str());
//
//if (iter != NameToIndex.end())
//{//find
//	cout << "find" << endl;
//}
//else
//{//no find
//	cout << "no find" << endl;
//}
//strBatch = "abc";
//
//if (strBatch == "")
//{
//	cout << "yes" << endl;
//}
//else
//{
//	cout << "no" << endl;
//}
//
//nTemp = NameToIndex[strBatch.c_str()];

//------------------------------------------

//int i = 0;
//int nNum = -1;
//stData data;
//stData data2;
//data.Name = "abc";
//data.len = 10;
//
//mapValue3["b"] = data;
//iter3 = mapValue3.begin();
//data2 = iter3->second;
//
//cout << data2.Name 
//<< "   , "
//<< data2.len
//<< endl;
//
//
//
//
////NameToIndex["a"] = 1;
////NameToIndex["b"] = 2;
////cout << "main : " << NameToIndex["a"] << endl;
////nNum = NameToIndex["c"];
////nNum = NameToIndex["a"];
////特性个人数据
//vecLive2.push_back("a1");
//vecLive2.push_back("a2");
//
//mapValue1["a1"] = "-1";
//mapValue1["a2"] = "-2";
//NameToIndex["a1"] = 1;
//NameToIndex["a2"] = 1;
////公共数据
//vecLive3.push_back("d");
//vecLive3.push_back("a2");
//vecLive3.push_back("c");
//
//mapValue2["d"] = "1";
//mapValue2["a2"] = "2";
//mapValue2["c"] = "3";
//for (i = 0; i < vecLive3.size(); i++)
//{
//	nNum = NameToIndex[vecLive3[i]];
//	//cout << nNum << endl;
//	if (!nNum)
//	{
//
//		vecLive2.push_back(vecLive3[i]);
//	}
//}
//
//iter2 = mapValue2.begin();
//
//for (;iter2 != mapValue2.end(); iter2++)
//{
//	nNum = NameToIndex[iter2->first];
//	if (!nNum)
//	{
//		mapValue1[iter2->first] =  iter2->second;
//		//vecLive2.push_back(vecLive3[i]);
//	}
//
//	//cout << iter2->first << endl;
//
//
//	//cout << iter2->first 
//	//	<< " , " 
//	//	<< iter2->second
//	//	<< endl;
//}
//
////最终打印内容，合集
//for (i = 0; i < vecLive2.size(); i++)
//{
//	//cout << vecLive2[i] << endl;
//}
//
//for (iter2 = mapValue1.begin();iter2 != mapValue1.end(); iter2++)
//{
//	cout << iter2->first 
//		<< " , " 
//		<< iter2->second
//		<< endl;
//}


////*==========================================================
//// * Function   : isnull
//// * Description: 验证字符串是否为空
//// * Input      : const char *d_str   输入字符串
//// * Output     :
//// * Return     : 0       字符串为空
//// *         1    字符串不为空
//// * Author     : zhouwm
//// * Date       : 2004/08/16
//// ==========================================================*/
//int isnull (const char *d_str )
//{
//    if (d_str == NULL || d_str[0] == '\0' )
//        return(0);             
//   
//    return(1);
//}
//
////*==========================================================
//// * Function   : length
//// * Description: 取字符串长度
//// *              与Oracle的 length函数功能类似
//// * Input      : char *str   输入字符串
//// * Output     :
//// * Return     : >0      字符串长度
//// *              0       字符串为空
//// * Author     : zhouwm
//// * Date       : 2004/08/16
//// ==========================================================*/
//int length (const char *str )
//{
//    const char     *p;
//
//    if (isnull (str) == 0)
//    {
//        return(0);
//    }
//
//    for(p = str; * p != '\0' ; p++);
//
//    return(p - str);
//}
//
// /*==========================================================
// * Function   : instr
// * Description: 查找字符，与 Oracle的instr 函数功能类似
// * Input      : const char * src 源字符串
// *              const char  c    待查找字符
// * Output     :
// * Return     : >0      成功
// *              -1      输入为空
// * Author     : zhouwm
// * Date       : 2004/08/16
// ==========================================================*/
//int   instr (const char * src ,const  char c )
//{
//    const char *p;
//    int i =0,j=0, ilen=0,ipos =1,inth=1;
//
//    if(isnull (src) == 0)
//        return(-1);
//   
//    j = 0;
//    //ipos = pos;
//    //inth = nth;
//    p = src ;
//    if (inth <= 0)
//        inth = 1;
//
//    ilen = length (p);
//    if (abs (ipos) > ilen)
//        return 0;
//
//    if (ipos >= 0 )
//    {
//        if (ipos > 0)
//            ipos = ipos -1;
//       
//        for (i = ipos; i < ilen ;i++)
//        {
//            if (*(p +i) == '\0')
//                return 0;
//            if (*(p +i) == c)
//            {
//                j = j + 1;
//                if (j == inth)
//                    return (i +1);
//            }
//        }
//    }
//    else
//    {
//        ilen = ilen - abs( ipos);
//        for (i = ilen - 1; i >= 0;i --)
//        {
//           
//            if (*(p +i) == c)
//            {
//                j = j + 1;
//                if (j == inth)
//                    return (i +1);
//            }
//        }
//    }
//    return 0;
//}
//
//
//int   instrEx (const char * src ,const  char c , const int pos = 1, const int appearPosition = 1)
//{
//	const char *p;
//	int i =0,j=0, ilen=0,ipos =1,inth=1;
//
//	if(isnull (src) == 0)
//		return(-1);
//
//	j = 0;
//	ipos = pos ;
//	inth = appearPosition ;
//	p = src ;
//	if (inth <= 0)
//		inth = 1;
//
//
//	ilen = length (p);
//	if (abs (ipos) > ilen)
//		return 0;
//
//	if (ipos >= 0 )
//	{
//		if (ipos > 0)
//			ipos = ipos -1;
//
//		for (i = ipos; i < ilen ;i++)
//		{
//			if (*(p +i) == '\0')
//				return 0;
//			if (*(p +i) == c)
//			{
//				j = j + 1;
//				if (j == inth)
//					return (i +1);
//			}
//		}
//	}
//	else
//	{
//		ilen = ilen - abs( ipos);
//		for (i = ilen; i >= 0;i --)
//		{
//
//			if (*(p +i) == c)
//			{
//				j = j + 1;
//				if (j == inth)
//					return (i + 1);
//			}
//		}
//	}
//	return 0;
//}

//void TestInstr ()
//{
//	//测试内容
//	char szBuf [256] = "1234";
//	char c = '1';
//	int nRet = 0;
//
//	//    nRet = instr(szBuf, c);
//	//    cout << nRet << endl;
//
//	//c = 'r';
//	//nRet = instrEx(szBuf, c, -1, 2);
//	nRet = instrEx (szBuf, c, 0, 1);
//	cout << nRet << endl;
//
//}

//void testNullable();




// class Base {};
// class Derived: public Base {};

//Base b, *pb;
//pb = NULL;
//Derived d;

//cout << typeid(int).name() << endl
//	<< typeid(unsigned).name() << endl
//	<< typeid(long).name() << endl
//	<< typeid(unsigned long).name() << endl
//	<< typeid(char).name() << endl
//	<< typeid(unsigned char).name() << endl
//	<< typeid(float).name() << endl
//	<< typeid(double).name() << endl
//	<< typeid(string).name() << endl
//	<< typeid(Base).name() << endl
//	<< typeid(b).name()<<endl
//	<< typeid(pb).name()<<endl
//	<< typeid(Derived).name() << endl
//	<< typeid(d).name()<<endl
//	<< typeid(type_info).name() << endl;

// 	Base *pb2 = dynamic_cast<Base *>(new Derived);
// 	Base &b2 = d;
// 	Base *pb3 = &d;
// 	cout << typeid(pb2).name() <<endl//输出Base *
// 		<< typeid(b2).name()<<endl //输出Base
// 		<< typeid(pb3).name()<<endl//输出Base *
// 		<< typeid(*pb3).name()<<endl;//输出Base
//测试异常模块
//void Func()
//{
//	//throw MyException();
//	throw Poco::MyException();
//}
//void testMyException()
//{
//	try
//	{
//		Func();
//	}
//	catch (MyException e)
//	{
//		cout << "MyException: "
//			<< e.what()
//			<< endl;
//	}
//	catch (Poco::MyException& e)
//	{
//		cout << "MyException: "
//			<< e.what()
//			<< endl;
//	}
//}


//void testNullable()
//{
//	Nullable<int> i;
//	Nullable<double> f;
//	Nullable<std::string> s;
//
//	assert (i.isNull());
//	assert (f.isNull());
//	assert (s.isNull());
//
//	i = 1;
//	f = 1.5;
//	s = "abc";
//
//	assert (!i.isNull());
//	assert (!f.isNull());
//	assert (!s.isNull());
//
//	assert (i == 1);
//	assert (f == 1.5);
//	assert (s == "abc");
//
//	i.clear();
//	f.clear();
//	s.clear();
//
//	assert (i.isNull());
//	assert (f.isNull());
//	assert (s.isNull());
//
//	Nullable<int> n1;
//	assert (n1.isNull());
//
//	assert (n1.value(42) == 42);
//	assert (n1.isNull());
//	assert (!(0 == n1));
//	assert (0 != n1);
//	assert (!(n1 == 0));
//	assert (n1 != 0);
//
//	try
//	{
//		int tmp = n1.value();
//		//fail("null value, must throw");
//	}
//	catch (Poco::NullValueException& e)
//	{
//// 		cout << e.what() << endl;
//// 		cout << e.className() << endl;
//// 		cout << e.name() << endl;
//// 		cout << e.message() << endl;
//	}
//
//	n1 = 1;
//	assert (!n1.isNull());
//	assert (n1.value() == 1);
//
//	Nullable<int> n2(42);
//	assert (!n2.isNull());
//	assert (n2.value() == 42);
//	assert (n2.value(99) == 42);
//
//	assert (!(0 == n2));
//	assert (0 != n2);
//	assert (!(n2 == 0));
//	assert (n2 != 0);
//
//	n1 = n2;
//	assert (!n1.isNull());
//	assert (n1.value() == 42);
//
//	std::ostringstream str;
//	str << n1;
//	assert (str.str() == "42");
//
//	n1.clear();
//	assert (n1.isNull());
//
//	str.str(""); str << n1;
//	assert (str.str().empty());
//
//	n2.clear();
//	assert (n1 == n2);
//	n1 = 1; n2 = 1;
//	assert (n1 == n2);
//	n1.clear();
//	assert (n1 < n2);
//	assert (n2 > n1);
//	n2 = -1; n1 = 0;
//	assert (n2 < n1);
//	assert (n2 != n1);
//	assert (n1 > n2);
//
//	NullType nd;
//	assert (n1 != nd);
//	assert (nd != n1);
//	n1.clear();
//	assert (n1 == nd);
//	assert (nd == n1);
//}

//template <typename T>
//T Div(T x, T y)
//{
//	if(y == 0)
//		throw y;//抛出异常
//	return x/y;
//}
//
//int x=5,y=0;
//double x1=5.5,y1=0.0;
//try
//{
//	//被检查的语句
//	std::cout<<x<<"/"<<y<<"="<<Div(x,y)<<std::endl;
//	std::cout<<x1<<"/"<<y1<<"="<<Div(x1,y1)<<std::endl;
//}
//catch(int)//异常类型
//{
//	std::cout<<"除数为0,计算错误！"<<std::endl;//异常处理语句
//}
//catch(double)//异常类型
//{
//	std::cout<<"除数为0.0,计算错误！"<<std::endl;//异常处理语句
//}

//共享智能指针测试
//namespace
//{
//	class TestObject
//	{
//	public:
//		TestObject(const std::string& data): _data(data)
//		{
//			++_count;
//		}
//
//		virtual ~TestObject()
//		{
//			--_count;
//		}
//
//		const std::string& data()
//		{
//			return _data;
//		}
//
//		static int count()
//		{
//			return _count;
//		}
//
//	private:
//		std::string _data;
//		static int _count;
//	};
//
//	int TestObject::_count = 0;
//
//	class DerivedObject: public TestObject
//	{
//	public:
//		DerivedObject(const std::string& s, int i): TestObject(s), _number(i)
//		{
//		}
//
//		int number() const
//		{
//			return _number;
//		}
//
//	private:
//		int _number;
//	};
//};
////共享指针，支持自动释放,离开区域自动释放
//void Func(TestObject* pTO1)
//{
//	SharedPtr<TestObject> ptr1;
//	ptr1 = pTO1;
//	cout << ptr1->data() 
//		<< ", count = "
//		<< ptr1->count()
//		<< endl;
//}
//
//void testSharePtr()
//{
//	SharedPtr<TestObject> ptr1;
//	//assertNull(ptr1.get());
//	TestObject* pTO1 = new TestObject("one");
//	TestObject* pTO2 = new TestObject("two");
//
//	//Func(pTO1);
//
//	if (pTO2 < pTO1)
//	{
//		TestObject* pTmp = pTO1;
//		pTO1 = pTO2;
//		pTO2 = pTmp;
//	}
//	assert (pTO1 < pTO2);
//	ptr1 = pTO1;
//	assert (ptr1.referenceCount() == 1);
//	SharedPtr<TestObject> ptr2 = pTO2;
//	SharedPtr<TestObject> ptr3 = ptr1;
//	assert (ptr1.referenceCount() == 2);
//	SharedPtr<TestObject> ptr4;
//	assert (ptr1.get() == pTO1);
//	assert (ptr1 == pTO1);
//	assert (ptr2.get() == pTO2);
//	assert (ptr2 == pTO2);
//	assert (ptr3.get() == pTO1);
//	assert (ptr3 == pTO1);
//
//	assert (ptr1 == pTO1);
//	assert (ptr1 != pTO2);
//	assert (ptr1 < pTO2);
//	assert (ptr1 <= pTO2);
//	assert (ptr2 > pTO1);
//	assert (ptr2 >= pTO1);
//
//	assert (ptr1 == ptr3);
//	assert (ptr1 != ptr2);
//	assert (ptr1 < ptr2);
//	assert (ptr1 <= ptr2);
//	assert (ptr2 > ptr1);
//	assert (ptr2 >= ptr1);
//
//	ptr1.swap(ptr2);
//	//assert (ptr2 < ptr1);
//	cout << ptr1->data() << ptr1->count() << endl;
//	ptr2.swap(ptr1);
//	cout << ptr1->data() << ptr1->count() << endl;
//
//	cout << TestObject::count() << endl;
//	ptr1 = 0;
//	ptr2 = 0;
//	ptr3 = 0;
//	cout << TestObject::count() << endl;
//
//	{
//		SharedPtr<TestObject> ptr = new TestObject("");
//		assert (TestObject::count() == 1);
//	}
//	assert (TestObject::count() == 0);
//}
//
//void testImplicitCast()
//{
//	{
//		// null assign test
//		SharedPtr<DerivedObject> ptr2;
//		//assertNull(ptr2.get());
//		DerivedObject* pPtr2 = ptr2.get();
//		assert(0 == pPtr2);
//		SharedPtr<TestObject> ptr1 = ptr2;
//	}
//	{
//		SharedPtr<DerivedObject> ptr2(new DerivedObject("test", 666));
//		assert (TestObject::count() == 1);
//		SharedPtr<TestObject> ptr1 = ptr2;
//		assert (TestObject::count() == 1);
//	}
//	assert (TestObject::count() == 0);
//	SharedPtr<TestObject> ptr1 = new DerivedObject("test", 666);
//	assert (TestObject::count() == 1);
//	ptr1 = 0;
//	assert (TestObject::count() == 0);
//}
//
//
//void testExplicitCast()
//{
//	SharedPtr<TestObject> ptr1 = new DerivedObject("test", 666);
//	SharedPtr<DerivedObject> ptr2 = ptr1.cast<DerivedObject>();
//	assert (ptr2.get() != 0);
//
//	// cast the other way round must fail
//	ptr1 = new TestObject("test");
//	assert (TestObject::count() == 2);
//	ptr2 = ptr1.cast<DerivedObject>();
//	assert (TestObject::count() == 1);
//	assert (ptr2.get() == 0);
//}


//空指针可以这么判断
//struct Parent
//{
//	Parent() { i = -1; }
//	virtual ~Parent() { i= -2; }
//
//	int i;
//};
//
//struct Parent *pParent = NULL;
//
//if (pParent == NULL || (pParent->i = -1))
//{
//
//}

//int nCount = 0;
//NameToIndex["king"] = 1;
//NameToIndex["b"]    = 2;
//NameToIndex["c"]    = 3;
//
//cout << NameToIndex["b"] << endl;
//cout << NameToIndex["c"] << endl;
//
//iter = NameToIndex.find("d");
//if (iter == NameToIndex.end())
//{
//	nCount = NameToIndex.size();
//	cout << "no find"
//		<< endl
//		<< nCount
//		<< endl;
//
//	NameToIndex["d"] = ++nCount;
//}
//else
//{
//	int nTemp = -9999;
//	nTemp = iter->second;
//	cout << "yes find, "
//		<< nTemp
//		<< endl;
//}
//
//cout << NameToIndex["d"] << endl;

//struct Parent
//{
//	Parent() { i = -1; }
//	virtual ~Parent() { i= -2; }
//
//	static int i;
//};
//
//int Parent::i = 0;
//
//struct Parent t1;
//cout << Parent::i << endl;

//测试原子加
//AtomicCounter ac;
//
//assert (ac.value() == 0);
//assert (ac++ == 0);
//assert (ac-- == 1);
//assert (++ac == 1);
//assert (--ac == 0);

//测试名字内容
//NameToIndex["king"] = 0;
//NameToIndex["b"]    = 1;
//NameToIndex["c"]    = 2;
//
//cout << NameToIndex["b"] << endl;
//cout << NameToIndex["c"] << endl;
//
//iter = NameToIndex.find("king");
//if (iter == NameToIndex.end())
//{
//	cout << "no find" << endl;
//}
//else
//{
//	cout << "yes find" << endl;
//}

//Mutex _mutex;
//Mutex::ScopedLock lock(_mutex);

//在C++中，mutable也是为了突破const的限制而设置的。被mutable修饰的变量，
//将永远处于可变的状态，即使在一个const函数中。

//class ClxTest
//{
//public:
//	ClxTest();
//	~ClxTest();
//
//	void Output() const;
//	int GetOutputTimes() const;
//
//private:
//	mutable int m_iTimes;
//	//int m_iTimes;
//};
//
//ClxTest::ClxTest()
//{
//	m_iTimes = 0;
//}
//ClxTest::~ClxTest()
//{}
//void ClxTest::Output() const
//{
//	cout << "Output for test!" << endl;
//	m_iTimes++;
//}
//
//int ClxTest::GetOutputTimes() const
//{
//	return m_iTimes;
//}
//
//void OutputTest(const ClxTest& lx)
//{
//	cout << lx.GetOutputTimes() << endl;
//	lx.Output();
//	cout << lx.GetOutputTimes() << endl;
//}
//
//ClxTest t1;
//OutputTest(t1);

//class A
//{
//public:
//	A()
//	{
//		cout << "A()" << endl;		
//	}
//	void Func()
//	{
//		cout << "Func" << endl;
//	}
//	void Abc()
//	{
//		cout << "Abc" << endl;
//	}
//};
//
//A t1;
//t1.Func();
//LockTest<A> t3(t1);
//t3.Open();

//vecLive2.push_back("abc");
//vecLive2.push_back("king");
//vecLive[0] = vecLive2;
//vecLive2.clear();
//
//vecLive2.push_back("123");
//vecLive2.push_back("234");
//vecLive[1] = vecLive2;
//
//cout << vecLive[0][1] << endl;
//cout << vecLive[1][0] << endl;

//int i, j;
//char szBuf[256] = {0};
//map<int, string>::iterator iter;

//maplive[0][0] = "abc";
//maplive[0][1] = "king";

//for (i = 0; i < 5; i++)
//{
//	for (j = 0; j < 10; j++)
//	{
//		sprintf(szBuf, "i = %d, j= %d", i, j);
//		maplive[i][j] = szBuf;
//	}		
//}

//iter = maplive[2].begin();

//for (;iter != maplive[2].end(); iter++)
//{

//	//cout << **iter << endl;
//}

//for (i = 0; i < maplive[2].size(); i++)
//{
//	cout << maplive[2][i] << endl;
//}

//cout << "----------------------------------" << endl;

//maplive2 = maplive[2];

//for (i = 0; i < maplive2.size(); i++)
//{
//	cout << maplive2[i] << endl;
//}

//cout << "----------------------------------" << endl;

//maplive2 = maplive[1];

//for (i = 0; i < maplive2.size(); i++)
//{
//	cout << maplive2[i] << endl;
//}

//cout << "----------------------------------" << endl;

///////////////////验证vector正确性//////////////

//for (i = 0; i < 5; i++)
//{
//	for (j = 0; j < 10; j++)
//	{
//		sprintf(szBuf, "i = %d, j= %d", i, j);
//		vecLive[i].push_back(szBuf);
//	}		
//}

//vecLive2 = vecLive[3];

//for (i = 0; i < vecLive2.size(); i++)
//{
//	cout << vecLive2[i] << endl;
//}

//NameToIndex["king"] = 0;
//NameToIndex["b"]    = 1;
//NameToIndex["c"]    = 2;
//
//cout << NameToIndex["b"] << endl;
//cout << NameToIndex["c"] << endl;



// class engine {
// public :
// 	void start() {cout << "engine->start" << endl;}
// 	void move() {cout << "engine->move" << endl;}
// 	void stop() {cout << "engine->stop" << endl;}
// };
// class wheel {
// public :
// 	void start() {cout << "wheel->start" << endl;}
// 	void move() {cout << "wheel->move" << endl;}
// 	void stop() {cout << "wheel->stop" << endl;}
// };
// class car : private engine, private wheel {
// public :
// 	void start();
// 	void move();
// 	void stop();
// };
// void car::start() {
// 	engine::start();
// 	wheel::start();
// }
// void car::move() {
// 	engine::move();
// 	wheel::move();
// }
// void car::stop() {
// 	engine::stop();
// 	wheel::stop();
// }

//car ca;
//ca.start();
//ca.move();
//ca.stop();
