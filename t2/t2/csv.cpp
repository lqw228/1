//20140624 周位壬 csv值中‘，’字符替换为不可见字符， 修改单：20140620044
#include "csv.h"
#include "s_libpublic_uft.h"
#include "s_liblogpublic.h"
#include "errno.h" //lint !e451
#include "string.h"

#define _stricmp strcasecmp

const char REPLACESYMBOL = 24;
//20150123 陈静 csv值中'\n'字符替换为不可见字符， 修改单：20150123029
const char REPLACENEWLINESYMBOL = 25;
//M201504170001 20150415 陈静 csv值中'\r'字符替换为不可见字符
const char REPLACEENTERSYMBOL = 26;
/**
* 字段名排序比较算子
*/
bool NameCmp(const CSVFieldName2Index& elem1, const CSVFieldName2Index& elem2)
{
	return _stricmp(elem1.lpName, elem2.lpName) < 0;
}

CCSVHelper::CCSVHelper() : m_lpFile(NULL), m_nFieldCount(0)
{
	memset(m_lpFieldInfos, 0, CSV_MAX_FIELDS*sizeof(CSVFIELD));
}

CCSVHelper::~CCSVHelper()
{

	if (m_lpFile)
		fclose(m_lpFile);
}
int CCSVHelper::BeginCreate(const char* szFileName)
{
	if (!szFileName)
	{
		return 1;
	}

	if (m_lpFile)
	{
		fclose(m_lpFile);
	}


	m_nFieldCount = 0;

	if (NULL == (m_lpFile = fopen(szFileName, "wb+")))
	{
		return 2;
	}

	return 0;
}

int CCSVHelper::AddField(const char* szName)
{

	size_t nameLen = strlen(szName);

	memset(m_lpFieldInfos + m_nFieldCount, 0, sizeof(CSVFIELD));

	char* p = (char*)(m_lpFieldInfos + m_nFieldCount);
	for (size_t i = 0; i < nameLen; ++i)
	{
		if ((szName[i] >= 'a') && (szName[i] <= 'z'))
		{
			*p++ = szName[i] - 'a' + 'A';
		}
		else
		{
			*p++ = szName[i];
		}
	}
	++m_nFieldCount;
	return 0;

}

int CCSVHelper::EndCreate()
{
	if (!m_lpFile)
	{
		return 1;
	}

	if (0 == m_nFieldCount)
	{
		fclose(m_lpFile);
		m_lpFile = NULL;
		return 2;
	}

	char sDataBuffer[MAX_RECORD_LEN];
	sDataBuffer[0] = 0;

	for (uint32_t i = 0; i < m_nFieldCount; i++)
	{
		const char* szName = (const char*)m_lpFieldInfos[i].m_sName;
		if (i == m_nFieldCount - 1)
		{
			sprintf(sDataBuffer, "%s%s", sDataBuffer, szName);
		}
		else
		{
			sprintf(sDataBuffer, "%s%s%c", sDataBuffer, szName, ',');
		}
	}
	fprintf(m_lpFile, "%s\n", sDataBuffer);

	sDataBuffer[0] = 0;

	fclose(m_lpFile);
	m_lpFile = NULL;

	return 0;
}

CCSVField::CCSVField() :m_nCurNoInBatch(0), m_nBatchCount(1),
m_lpBufs(NULL), m_nNo(0)
{
	memset(&m_FieldInfo, 0, sizeof(CSVFIELD));
}

CCSVField::~CCSVField()
{
	try
	{
		ClearBuffer();

		m_lpBufs = NULL;
	}
	catch (...)
	{

	}

}
int CCSVField::SetValue(int32_t nValue)
{
	//临时缓存
	char szVal[MAX_FIELD_CASHE_LEN];
	size_t nLength = sprintf(szVal, "%d", nValue);

	assert(m_lpBufs);
	if (m_lpBufs == NULL)
	{
		return -1;
	}
	char* pstr = m_lpBufs[m_nCurNoInBatch];

	if (pstr == NULL || (pstr&&nLength>strlen(pstr)))
	{
		pstr = (char*)realloc(pstr, (nLength + 1));
		m_lpBufs[m_nCurNoInBatch] = pstr;
		//pstr[nLength]=0;
	}

	assert(pstr);
	if (pstr == NULL)
	{
		return -1;
	}
	memcpy(pstr, szVal, nLength + 1);
	pstr[nLength] = 0;
	return 0;
}
int CCSVField::SetValue(uint32_t nValue)
{
	//临时缓存
	char szVal[MAX_FIELD_CASHE_LEN];
	size_t nLength = sprintf(szVal, "%u", nValue);

	assert(m_lpBufs);
	if (m_lpBufs == NULL)
	{
		return -1;
	}
	char* pstr = m_lpBufs[m_nCurNoInBatch];

	if (pstr == NULL || (pstr&&nLength>strlen(pstr)))
	{
		pstr = (char*)realloc(pstr, nLength + 1);
		m_lpBufs[m_nCurNoInBatch] = pstr;
		//pstr[nLength]=0;
	}
	assert(pstr);
	if (pstr == NULL)
	{
		return -1;
	}
	memcpy(pstr, szVal, nLength + 1);
	pstr[nLength] = 0;
	return 0;
}
//int CCSVField::SetValue(int64_t nValue)
//{
//	//临时缓存
//	char szVal[MAX_FIELD_CASHE_LEN];
//	int nLength = sprintf(szVal, "%ld", nValue);
//
//	char* pstr=m_lpBufs[m_nCurNoInBatch];
//
//	if (pstr==NULL||(pstr&&nLength>strlen(pstr)))
//	{
//		pstr=(char*)realloc(pstr,nLength+1);
//		m_lpBufs[m_nCurNoInBatch]=pstr;
//		//pstr[nLength]=0;
//	}
//	memcpy(pstr, szVal, nLength+1);
//	pstr[nLength]=0;
//	return 0;
//}
int CCSVField::SetValue(uint64_t nValue)
{
	//临时缓存
	char szVal[MAX_FIELD_CASHE_LEN];
	size_t nLength = sprintf(szVal, "%lu", nValue);

	assert(m_lpBufs);
	if (m_lpBufs == NULL)
	{
		return -1;
	}
	char* pstr = m_lpBufs[m_nCurNoInBatch];

	if (pstr == NULL || (pstr&&nLength>strlen(pstr)))
	{
		pstr = (char*)realloc(pstr, nLength + 1);
		m_lpBufs[m_nCurNoInBatch] = pstr;
		//pstr[nLength]=0;
	}
	assert(pstr);
	if (pstr == NULL)
	{
		return -1;
	}
	memcpy(pstr, szVal, nLength + 1);
	pstr[nLength] = 0;
	return 0;
}
int CCSVField::SetValue(double nValue)
{
	//临时缓存
	char szVal[MAX_FIELD_CASHE_LEN];
	size_t nLength = sprintf(szVal, "%lf", nValue);

	assert(m_lpBufs);
	if (m_lpBufs == NULL)
	{
		return -1;
	}
	char* pstr = m_lpBufs[m_nCurNoInBatch];

	if (pstr == NULL || (pstr&&nLength>strlen(pstr)))
	{
		pstr = (char*)realloc(pstr, nLength + 1);
		m_lpBufs[m_nCurNoInBatch] = pstr;
		// pstr[nLength]=0;
	}
	assert(pstr);
	if (pstr == NULL)
	{
		return -1;
	}
	memcpy(pstr, szVal, nLength + 1);
	pstr[nLength] = 0;
	return 0;
}
int CCSVField::SetValue(const char* szValue)
{
	if (!szValue)
		return SVE_WRONG_PARAM;

	size_t nLength = (int)strlen(szValue);

	assert(m_lpBufs);
	if (m_lpBufs == NULL)
	{
		return -1;
	}
	char* pstr = m_lpBufs[m_nCurNoInBatch];

	if (pstr == NULL || (pstr&&nLength>strlen(pstr)))
	{
		pstr = (char*)realloc(pstr, nLength + 1);
		m_lpBufs[m_nCurNoInBatch] = pstr;

	}
	assert(pstr);
	if (pstr == NULL)
	{
		return -1;
	}
	memcpy(pstr, szValue, nLength + 1);
	pstr[nLength] = '\0';

	char c;
	char *p = pstr;
	while (1)
	{
		c = *p;
		if (0 == c) break;
		if (',' == c)
		{
			*p = REPLACESYMBOL;
		}
		//20150123 陈静 csv值中'\n'字符替换为不可见字符， 修改单：20150123029
		else if ('\n' == c)
		{
			*p = REPLACENEWLINESYMBOL;
		}
		else if ('\r' == c)
		{
			*p = REPLACEENTERSYMBOL;
		}
		p++;
	}
	return 0;
}
int CCSVField::SetValue(const char* szValue, int iLen)
{
	if (!szValue)
		return SVE_WRONG_PARAM;

	size_t nLength = iLen;

	assert(m_lpBufs);
	if (m_lpBufs == NULL)
	{
		return -1;
	}
	char* pstr = m_lpBufs[m_nCurNoInBatch];

	if (pstr == NULL || (pstr&&nLength>strlen(pstr)))
	{
		pstr = (char*)realloc(pstr, nLength + 1);
		m_lpBufs[m_nCurNoInBatch] = pstr;

	}

	assert(pstr);
	if (pstr == NULL)
	{
		return -1;
	}
	pstr[nLength] = 0;
	memcpy(pstr, szValue, iLen);
	return 0;
}
void CCSVField::ClearBuffer()
{
	if (m_lpBufs)
	{
		for (int i = 0; i<m_nBatchCount; i++)
		{

			free(m_lpBufs[i]);

		}
		delete m_lpBufs;
		m_lpBufs = NULL;
	}

}

void CCSVField::CreateBuffer(int iBatchSize)
{

	ClearBuffer();

	m_lpBufs = new(std::nothrow) char*[iBatchSize];

	for (int i = 0; i<iBatchSize; i++)
	{
		m_lpBufs[i] = NULL;
	}

}
CSV::CSV() :m_lpFile(NULL), m_nRecordNo(0), m_nNoInBatch(0),
m_iFieldCount(0), m_nBatchCount(10), m_lpFieldName2Index(0),
m_nRecordCount(0), m_nBufRecNo0(0), m_nBufRecNo1(1),
m_bEOF(false), m_bBOF(false)
{
	m_nAccessFlags = (uint32_t)modeBrowse;
	m_nAppendRecCount = 0;

	LineDataBuffer[0] = 0;
	WriteDataBuffer[0] = 0;
	CurLineFieldsBuffer[0] = NULL;
}

CSV::~CSV()
{
	try
	{
		CloseFile();
		m_lpFile = NULL;
		delete[] m_lpFieldName2Index;
		m_lpFieldName2Index = NULL;
	}
	catch (...)
	{

	}

}

int CSV::OpenFile(const char *szFileName, uint32_t nOpenFlags, int iBatchSize)
{

	CloseFile();

	if (!szFileName || !strlen(szFileName) || iBatchSize<1)
		return -1;
	char szAccess[4] = { 'r', 'b', '+', '\0' };
	//char szAccess[4] = {'r', '+', '\0'};

	if (NULL == (m_lpFile = fopen(szFileName, const_cast<const char *>(szAccess))))
	{
		printf("open file: %s, access mode: %s, fail, error code: %d, %s\n", \
			szFileName, szAccess, errno, strerror(errno));
		LOGERROR(-1, "open file: " << szFileName << "access mode:" << szAccess << ", fail, error code:" << errno << ", " << strerror(errno));
		return -1;
	}

	if (GetFieldNames() == 0)
	{
		//统计记录行数
		int sizeBuffer = sizeof(LineDataBuffer);
		while (!feof(m_lpFile))
		{

			if (fgets(LineDataBuffer, sizeBuffer, m_lpFile))
			{
				m_nRecordCount++;
			}

		}
	}
	else
	{
		//printf("CSV::OpenFile, GetFieldNames() error, %s\n",  szFileName);
		LOGERROR(-1, "文件[" << szFileName << "]解析字段头失败" << m_nRecordCount);
		return -1;
	}
	CreateBuffer(iBatchSize);

	if (NULL == (m_lpFieldName2Index = new(std::nothrow) CSVFieldName2Index[m_iFieldCount]))
	{
		return -2;
	}

	for (int i = 0; i < m_iFieldCount; ++i)
	{
		m_lpFieldName2Index[i].lpName = m_lpFields[i].GetName();
		m_lpFieldName2Index[i].iIndex = m_lpFields[i].GetNo();
	}

	std::sort(m_lpFieldName2Index, m_lpFieldName2Index + m_iFieldCount, NameCmp);
	//SetBatchSize(iBatchSize);


	//fseek(m_lpFile,0,SEEK_SET);

	LineDataBuffer[0] = 0;
	WriteDataBuffer[0] = 0;

	MoveFirst();
	return 0;
}

void CSV::CloseFile()
{
	if (m_lpFile)
	{
		fclose(m_lpFile);
	}
	m_lpFile = NULL;
}

void CSV::MoveFirst()
{
	if (m_nRecordCount>0)
	{
		(void)ReadRecord(0);

		m_nRecordNo = 0;
		m_bEOF = false;
		m_bBOF = false;
	}
	else
	{
		m_bEOF = true;
		m_bBOF = true;
	}
}

void CSV::MoveNext()
{
	if (m_nRecordNo + 1 < m_nRecordCount)
	{
		//如果是缓存的最后一个，则读取下一段数据
		if (m_nBufRecNo1 <= m_nRecordNo + 1)
		{
			//ReadRecord(m_nRecordNo + 1);
			//uint64 nTime=0;
			//uint64 nTimeVol=0;
			//nTime=FBASE2_NAMESPACE::GetTime();
			//LOGDETAIL("ReadRecordOnIndex "<<nTime);
			(void)ReadRecordOnIndex(m_nRecordNo + 1);
			//nTimeVol=nTime;
			//nTime=FBASE2_NAMESPACE::GetTime();
			//nTimeVol=nTime-nTimeVol;
			//LOGDETAIL("ReadRecordOnIndex "<<nTime<<" VOL "<<nTimeVol);

			for (int nIndex = 0; nIndex < m_iFieldCount; nIndex++)
			{
				m_lpFields[nIndex].m_nCurNoInBatch = m_nNoInBatch;
			}
		}
		else//如果缓存数据已经关读取，只移动定位序号
		{
			m_nNoInBatch += 1;
			for (int nIndex = 0; nIndex < m_iFieldCount; nIndex++)
			{
				m_lpFields[nIndex].m_nCurNoInBatch = m_nNoInBatch;
			}
		}
		m_nRecordNo++;
		m_bBOF = false;
		m_bEOF = false;
	}
	else
	{
		m_bEOF = true;
	}
}
void CSV::MoveLast()
{
	if (m_nRecordCount>0)
	{
		if (m_nBufRecNo1 < m_nRecordCount)
		{
			if ((uint32_t)m_nBatchCount >= m_nRecordCount)
			{
				(void)ReadRecord(0);
				// Rec2Buf(m_head.RecSize*(m_nRecordCount-1));
			}
			else
			{
				(void)ReadRecord((int)m_nRecordCount - m_nBatchCount);
				// Rec2Buf(m_head.RecSize*(m_nBatchCount-1));
			}
		}
		else
		{
			m_nNoInBatch = m_nBufRecNo1 - m_nBufRecNo0 - 1;
			//Rec2Buf(m_head.RecSize * m_nNoInBatch);
		}

		m_nRecordNo = m_nRecordCount - 1;
		m_bEOF = false;
		m_bBOF = false;
	}
	else
	{
		m_bEOF = true;
		m_bBOF = true;
	}
}
bool CSV::Append(int nAppendRecCount)
{
	if (NULL == m_lpFile || nAppendRecCount > m_nBatchCount || nAppendRecCount < 1)
	{
		return false;
	}
	m_nBufRecNo0 = m_nRecordCount;
	m_nBufRecNo1 = m_nBufRecNo0 + nAppendRecCount;
	m_nAppendRecCount = nAppendRecCount;
	m_nNoInBatch = 0;
	m_nAccessFlags = (uint32_t)modeAppend;

	for (int nIndex = 0; nIndex < m_iFieldCount; nIndex++)
	{
		m_lpFields[nIndex].m_nBatchCount = nAppendRecCount;
		m_lpFields[nIndex].m_nCurNoInBatch = m_nNoInBatch;
	}

	//m_nBatchCount = nAppendRecCount;

	return 0;
}

bool CSV::AppendNext()
{
	if (m_nAccessFlags != (uint32_t)modeAppend || m_nNoInBatch + 1 >= (uint32_t)m_nBatchCount)
	{
		return false;
	}
	m_nNoInBatch += 1;
	for (int nIndex = 0; nIndex < m_iFieldCount; nIndex++)
	{
		m_lpFields[nIndex].m_nCurNoInBatch = m_nNoInBatch;
	}
	return true;
	//return EndCreate();
}

//优化查找 iReadRecordNo	的所在行的方式
int CSV::ReadRecord(int iReadRecordNo)
{
	if (iReadRecordNo < 0 || (uint32_t)iReadRecordNo >= m_nRecordCount)
	{
		return -1;
	}

	assert(m_lpFile);
	if (m_lpFile == NULL)
	{
		return -1;
	}
	fseek(m_lpFile, 0, SEEK_SET);

	//移动文件指针记录到第一行记录，即实际的第2行
	if (NULL == fgets(LineDataBuffer, sizeof(LineDataBuffer), m_lpFile))
	{
		return -1;
	}

	size_t nReadCount = 0;
	size_t nReadCountInBatch = 0;
	while (!feof(m_lpFile) && nReadCountInBatch < (uint32_t)m_nBatchCount)
	{
		if (fgets(LineDataBuffer, sizeof(LineDataBuffer), m_lpFile))
		{

			if ((nReadCount++) >= (uint32_t)iReadRecordNo)
			{
				//LOGDETAIL("["<<nReadCount<<"]"<<LineDataBuffer<<"["<<nReadCountInBatch<<"]");
				int nFieldCount = SplitLineBuffer();

				if (LineBuffer2Fields(nFieldCount, (int)nReadCountInBatch) == -1)
				{
					return -1;
				}
				nReadCountInBatch++;
			}

		}
	}
	m_nBufRecNo0 = iReadRecordNo;
	m_nBufRecNo1 = m_nBufRecNo0 + nReadCountInBatch;

	m_nNoInBatch = 0;

	return 0;
}
int CSV::ReadRecordOnIndex(int iReadRecordNo)
{
	if (iReadRecordNo < 0 || (uint32_t)iReadRecordNo >= m_nRecordCount)
	{
		return -1;
	}

	//fseek(m_lpFile,0,SEEK_SET);

	//移动文件指针记录到第一行记录，即实际的第2行
	//if (NULL== fgets(LineDataBuffer,sizeof(LineDataBuffer),m_lpFile))
	//{
	//return -1;
	//}

	//size_t nReadCount=0;
	size_t nReadCountInBatch = 0;
	assert(m_lpFile);
	if (m_lpFile == NULL)
	{
		return -1;
	}
	while (!feof(m_lpFile) && nReadCountInBatch < (uint32_t)m_nBatchCount)
	{
		if (fgets(LineDataBuffer, sizeof(LineDataBuffer), m_lpFile))
		{

			//if ((nReadCount++) >=iReadRecordNo)
			{
				//LOGDETAIL("["<<nReadCount<<"]"<<LineDataBuffer<<"["<<nReadCountInBatch<<"]");

				//uint64 nTime=0;
				//uint64 nTimeVol=0;
				//nTime=FBASE2_NAMESPACE::GetTime();
				//LOGDETAIL("SPLITBEGIN :"<<nTime);
				int nFieldCount = SplitLineBuffer();
				//nTimeVol=nTime;
				//nTime=FBASE2_NAMESPACE::GetTime();
				//nTimeVol=nTime-nTimeVol;
				//LOGDETAIL("SPLITEND :"<<nTime<<" VOL :"<<nTimeVol);
				if (LineBuffer2Fields(nFieldCount, (int)nReadCountInBatch) == -1)
				{
					return -1;
				}
				//nTimeVol=nTime;
				//nTime=FBASE2_NAMESPACE::GetTime();
				//nTimeVol=nTime-nTimeVol;
				//LOGDETAIL("buff2field :"<<nTime<<"VOL :"<<nTimeVol);
				nReadCountInBatch++;
			}

		}
	}
	m_nBufRecNo0 = iReadRecordNo;
	m_nBufRecNo1 = m_nBufRecNo0 + nReadCountInBatch;

	m_nNoInBatch = 0;

	return 0;
}

int CSV::Post()
{

	if (m_lpFile == NULL)
	{
		return -1;
	}
	int nRow = 0;
	while ((uint32_t)nRow<m_nAppendRecCount)
	{
		WriteDataBuffer[0] = 0;
		for (int nIndex = 0; nIndex < m_iFieldCount; nIndex++)
		{
			m_lpFields[nIndex].m_nCurNoInBatch = m_nNoInBatch;
			if (m_lpFields[nIndex].m_lpBufs[nRow])
			{
				sprintf(WriteDataBuffer, "%s%s", WriteDataBuffer, m_lpFields[nIndex].m_lpBufs[nRow]);
			}
			else
			{
				sprintf(WriteDataBuffer, "%s%s", WriteDataBuffer, "");
			}


			if (nIndex != m_iFieldCount - 1)
			{
				sprintf(WriteDataBuffer, "%s%c", WriteDataBuffer, ',');
			}
		}
		nRow++;
		fprintf(m_lpFile, "%s\n", WriteDataBuffer);

		//LOGDETAIL(""<<WriteDataBuffer<<"");
		WriteDataBuffer[0] = 0;

	}


	return 0;
}

//////////////////////////////////////////////////////////////////////////

/// 增加是否区分大小写查找。
bool CSV::BinarySearch(LPCSVFieldName2Index a, size_t iLength, const char* key, size_t *uIndex, int iType)
{
	if (0 == iLength)
	{
		return false;
	}

	int low = 0;
	int high = iLength - 1;
	int mid = 0;
	int iRet = 0;

	while (low <= high)
	{
		mid = low + (high - low) / 2;

		if (iType == 0)
			iRet = _stricmp(a[mid].lpName, key);
		else
			iRet = strcmp(a[mid].lpName, key);

		if (iRet < 0)
		{
			low = mid + 1;
		}
		else if (iRet > 0)
		{
			high = mid - 1;
		}
		else
		{
			if (uIndex)
			{
				*uIndex = mid;
			}

			return true;
		}
	}

	return false;
}

int CSV::GetFieldNames()
{
	assert(m_lpFile);
	if (m_lpFile == NULL)
	{
		LOGERROR(-1, "m_lpFile == NULL");
		return -1;
	}

	fseek(m_lpFile, 0, SEEK_SET);
	if (fgets(LineDataBuffer, sizeof(LineDataBuffer), m_lpFile))//4/1/2014 zhouwr
	{
		int nCount = 0;
		nCount = SplitLineBuffer();
		if (nCount>0)
		{
			LineBuffer2Names(nCount);
		}
		else
		{
			LOGERROR(-1, "nCount = " << nCount);
			return -1;
		}
	}
	else
	{
		LOGERROR(-1, "LineDataBuffer = " << LineDataBuffer << ", m_lpFile=" << m_lpFile);
		return -1;
	}

	return 0;
}
void CSV::ClearBuffer()
{
	for (int i = 0; i<m_iFieldCount; i++)
	{
		m_lpFields[i].ClearBuffer();
	}

	/*std::vector<char*>::iterator it;
	for (it=m_lpRecordBuf.begin();it!=m_lpRecordBuf.end();it++)
	{
	free(*it);
	it=m_lpRecordBuf.erase(it);
	}	*/
}

void CSV::CreateBuffer(int iBatchSize)
{
	m_nBatchCount = iBatchSize;
	for (int i = 0; i<m_iFieldCount; i++)
	{
		m_lpFields[i].CreateBuffer(iBatchSize);
	}
}

int CSV::SplitLineBuffer()
{
	int nFieldCount = 0;

	size_t mGetLineReturnValue = strlen(LineDataBuffer);
	if ('\r' == LineDataBuffer[0] || '\n' == LineDataBuffer[0] || 0 == LineDataBuffer[0])
	{
		return nFieldCount;
	}

	if ('\r' == LineDataBuffer[mGetLineReturnValue] || '\n' == LineDataBuffer[mGetLineReturnValue])
	{
		LineDataBuffer[mGetLineReturnValue] = 0;
	}
	if ('\r' == LineDataBuffer[mGetLineReturnValue - 1] || '\n' == LineDataBuffer[mGetLineReturnValue - 1])
	{
		LineDataBuffer[mGetLineReturnValue - 1] = 0;
	}
	if ('\r' == LineDataBuffer[mGetLineReturnValue - 2] || '\n' == LineDataBuffer[mGetLineReturnValue - 2])
	{
		LineDataBuffer[mGetLineReturnValue - 2] = 0;
	}

	CurLineFieldsBuffer[nFieldCount] = LineDataBuffer;

	nFieldCount++;
	char c;
	char *p = LineDataBuffer;
	while (1)
	{
		c = *p;
		if (0 == c) break;
		if (',' == c)
		{


			*p = 0;
			CurLineFieldsBuffer[nFieldCount] = (p + 1);
			nFieldCount++;
		}
		else if (REPLACESYMBOL == c)
		{
			*p = ',';
		}
		else if (REPLACENEWLINESYMBOL == c)
		{
			*p = '\n';
		}
		else if (REPLACEENTERSYMBOL == c)
		{
			*p = '\r';
		}
		p++;
	}

	return nFieldCount;
}

void CSV::LineBuffer2Names(int nCount)
{
	m_iFieldCount = nCount;
	for (int i = 0; i<nCount; i++)
	{

		(void)hs_strncpy(m_lpFields[i].m_FieldInfo.m_sName, CurLineFieldsBuffer[i], sizeof(m_lpFields[i].m_FieldInfo.m_sName) - 1);	//设置列名称
		m_lpFields[i].m_FieldInfo.m_sName[sizeof(m_lpFields[i].m_FieldInfo.m_sName) - 1] = '\0';
		m_lpFields[i].m_nNo = i; // 设置列号
	}

}
int CSV::LineBuffer2Fields(int nFieldCount, int nReadNoInBatch)
{
	if (nFieldCount<m_iFieldCount)//!=
	{
		return -1;
	}

	uint32_t nLength = 0;

	for (int i = 0; i<nFieldCount; i++)
	{
		nLength = strlen(CurLineFieldsBuffer[i]);

		char* pstr = m_lpFields[i].m_lpBufs[nReadNoInBatch];
		if (pstr == NULL || (pstr&&nLength>strlen(pstr)))
		{
			pstr = (char*)realloc(pstr, nLength + 1);
			m_lpFields[i].m_lpBufs[nReadNoInBatch] = pstr;

		}
		assert(pstr);
		if (pstr == NULL)
		{
			return -1;
		}
		(void)hs_strncpy(pstr, CurLineFieldsBuffer[i], nLength);
		pstr[nLength] = '\0';

	}
	return 0;
}
