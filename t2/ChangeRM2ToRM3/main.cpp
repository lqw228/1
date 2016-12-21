#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int		uint32;

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

int ChangeFile(const char* pSrc, const char* pDest);


int main(int argc, char* argv[])
{
	int i = 0;
	int ret = 0;

	if (argc < 3)
	{
		printf("参数个数不够两个，第一个参数转换文件名，第二个参数输出文件名, %d\n", argc - 1);
		return -1;
	}

	for (i = 0; i< argc; i++)
	{
		printf("输入参数: %s\n", argv[i]);
	}

	ret = ChangeFile(argv[1], argv[2]);
	if (0 == ret)
	{
		printf("转换成功\n", ret); 
	}
	else
	{
		printf("转换失败, ret = %d\n", ret);
	}
	
  
	getchar();
	return 0;
}

int ChangeFile(const char* pSrc, const char* pDest)
{
	//写文件模块
	FILE* fp = fopen(pDest, "wb");
	//----读文件模块----
	FILE* m_lpReadFile = fopen(pSrc, "rb");
	int n = 0;
	PHEAD pHd = NULL;
	HEAD hd;
	int ch;
	int nCount;
	int num = 0;

	if (NULL == m_lpReadFile || NULL == fp)
	{
		return -1;
	}

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