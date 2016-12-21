/*************************************************************
*	Copyright(c) 2014, �������ӹɷ����޹�˾
*	All rights reserved.
*
*	�ļ����ƣ�	csv
*	�ļ���ʶ��  h
*	ժ    Ҫ����дCSV��ʽ���ݣ��ݲ�֧�ֱ༭���м����Ȳ���
*
*	��ǰ�汾��1.0
*	��    �ߣ�	zhouwr
*	�������ڣ�3/24/2014
*
****************************************************************/
#pragma once
#ifndef _CSV_H_
#define _CSV_H_
//#include "os/functional.h"
//#include "os/config_env.h"
//#include "os_common.h"

#include <vector>
#include <string.h>
#include <stdio.h>

#include <algorithm>

#define CSV_MAX_FIELDS 1024
//��ʽ����������ת�����ַ��������ַ���ת�������ֳ���
#define SVE_FORMAT_FAILED			-1
//����Խ���������ת�����ַ����󳬹��ֶγ���
#define SVE_LENGTH_OVERFLOW			-2
//��������
#define SVE_WRONG_PARAM				-3

#define MAX_RECORD_LEN 20460
#define MAX_FIELD_CASHE_LEN 10240
typedef char uint8_t ;
#pragma pack(1)
/**< CSV�ļ�ÿ�ֶνṹ */
typedef struct tagCSVField
{
	char		m_sName[128];
	uint8_t		Reserved[14];
}CSVFIELD, *LPCSVFIELD;

typedef struct CSV_FieldName2Index
{
	CSV_FieldName2Index()
	{
		lpName = NULL;
		iIndex = -1;
	}
	const char *lpName;
	int iIndex;
}CSVFieldName2Index, *LPCSVFieldName2Index;

#pragma pack()

class CCSVHelper
{
public:
	CCSVHelper();
	~CCSVHelper();

public:
	int BeginCreate(const char* fileName);
	int AddField(const char* name);
	int EndCreate();

private:
	/**
	* ����ÿ�δ���ļ�¼����
	* @param iBatchCount ����С
	*/
	//   inline void SetBatchSize(int iBatchCount)
	//   {
	//       if (m_lpFile && iBatchCount > 0)
	//           m_nBatchCount = iBatchCount;
	//   }
	//void ClearBuffer();

private:
	FILE*		m_lpFile;		/**< �ļ�ָ�� */
	/// ����csv�ļ��ֶε�����
	CSVFIELD m_lpFieldInfos[CSV_MAX_FIELDS];
	/// ��¼��ǰCSV�ļ����ֶ���
	uint32_t m_nFieldCount;

};

class CCSVField
{
	friend class CSV;
public:
public:
	CCSVField();
	~CCSVField();
	/**
	* ��ȡ����
	* @return const char*	����
	*/
	inline const char* GetName() const
	{
		return (const char*)m_FieldInfo.m_sName;
	}

	/**
	* ��ȡ�к�
	* @return int		�к�
	*/
	inline int GetNo() const
	{
		return m_nNo;
	}
	/**
	* ��ȡ������ֵ
	* @return ������ֵ
	*/
	inline int32_t AsInt() const
	{
		return atol(m_lpBufs[m_nCurNoInBatch]);
	}

	/**
	* ��ȡ������ֵ
	* @return ������ֵ
	*/
	/*inline int64_t AsInt64() const
	{
	int64_t ret=0;
	sscanf(m_lpBufs[m_nCurNoInBatch], "%l", &ret);
	return ret;
	}*/

	/**
	* ��ȡ�޷���������ֵ
	* @return �޷���������ֵ
	*/
	inline uint64_t AsUInt64() const
	{
		uint64_t ret = 0;
		sscanf(m_lpBufs[m_nCurNoInBatch], "%lu", &ret);
		return ret;
	}

	/**
	* ��ȡ�޷���������ֵ
	* @return �޷���������ֵ
	*/
	inline uint32_t AsUInt() const
	{
		uint32_t ret = 0;
		sscanf(m_lpBufs[m_nCurNoInBatch], "%u", &ret);
		return ret;
	}
	/**
	* ��ȡ��������ֵ
	* @return ��������ֵ
	*/
	inline double AsFloat() const
	{
		return atof(m_lpBufs[m_nCurNoInBatch]);
	}
	/**
	* ��ȡ�ַ�����ֵ
	* @return �ַ�����ֵ
	*/
	inline char AsChar() const
	{
		return *(m_lpBufs[m_nCurNoInBatch]);
	}

	/**
	* ��ȡ�ַ�������ֵ
	* @return �ַ�������ֵ
	*/
	inline const char* AsString() const
	{
		return m_lpBufs[m_nCurNoInBatch];
	}

	/**
	* ����������ֵ
	* @param nValue��Ҫ���õ�ֵ
	* @param 0��ʾ�ɹ���������ʾʧ��
	*/
	int SetValue(int32_t nValue);

	/**
	* ����������ֵ
	* @param nValue��Ҫ���õ�ֵ
	* @param 0��ʾ�ɹ���������ʾʧ��
	*/
	int SetValue(uint32_t nValue);

	/**
	* ����������ֵ
	* @param nValue��Ҫ���õ�ֵ
	* @param 0��ʾ�ɹ���������ʾʧ��
	*/
	//int SetValue(int64_t nValue);

	/**
	* ����������ֵ
	* @param nValue��Ҫ���õ�ֵ
	* @param 0��ʾ�ɹ���������ʾʧ��
	*/
	int SetValue(uint64_t nValue);

	/**
	* ����˫��������ֵ
	* @param lfValue	Ҫ���õ�ֵ
	* @param 0��ʾ�ɹ���������ʾʧ��
	*/
	int SetValue(double lfValue);

	/**
	* ������ֵ
	* @param szValue	Ҫ���õ�ֵ
	* @param 0��ʾ�ɹ���������ʾʧ��
	*/
	int SetValue(const char *szValue);

	/**
	* ������ֵ���ַ���������0��β��
	* @param szValue �ַ���
	* @param iLen	���õ��ַ�������
	* @param 0��ʾ�ɹ���������ʾʧ��
	*/
	int SetValue(const char *szValue, int iLen);
private:
	void ClearBuffer();

	void CreateBuffer(int iBatchSize);
private:
	int			m_nNo;/**< �к� */
	CSVFIELD	m_FieldInfo;/**< ����Ϣ */
	int m_nBatchCount;//
	int m_nCurNoInBatch; //��ǰ��������
	char** m_lpBufs; //�л���
};
class CSV
{
public:
	CSV();
	~CSV();
	int OpenFile(const char *fileName, uint32_t nOpenFlags, int iBatchSize = 10);
	void CloseFile();
	/** �ƶ����ļ���¼��ʼ
	* @return ��
	*/
	void MoveFirst();

	/** ����ڵ�ǰ��¼������ƶ�һ����¼
	* @return ��
	*/
	void MoveNext();

	/** �ƶ����ļ���¼ĩβ
	* @return ��
	*/
	void MoveLast();
	/**
	* �Ƿ����ļ�ĩβ
	* @return true��ʾ�ļ�ĩβ����֮false
	*/
	inline bool IsEOF()
	{
		return m_bEOF;
	}
	/** ���һ���������¼
	* @param nAppendRecCount Ҫ��ӵļ�¼��
	* @return true �ɹ�
	*/
	bool Append(int appendRecCount = 1);
	/** �����һ����¼
	* @return true �ɹ���false ʧ��
	*/
	bool AppendNext();

	/** �ύ�޸Ļ���ӵļ�¼
	* @return 0��ʾ�ɹ�������ʧ�ܣ�д�ļ���
	*/
	int Post();

	/**
	* ��ȡ�ֶ�ֵ����,�����GetField(int nIndex)������
	* @param szFieldName	�ֶ���
	* @return �ֶ�����Ӧ���ֶ�ֵָ��
	*/
	inline CCSVField* GetFieldPtr(const char *szFieldName)
	{
		if (!szFieldName || !m_lpFile)
		{
			return NULL;
		}
		size_t szIndex = 0;
		if (BinarySearch(m_lpFieldName2Index, m_iFieldCount, szFieldName, &szIndex))
		{
			int iIndex = m_lpFieldName2Index[szIndex].iIndex;
			if (iIndex >= 0 && iIndex < m_iFieldCount)
			{
				return &m_lpFields[iIndex];
			}
		}
		return NULL;
	}

	/**
	* ��ȡ�ֶ�ֵ��ָ��
	* @param nIndex	�ֶ��±�
	* @return �ֶ�����Ӧ���ֶ�ֵָ�룬���ʧ�ܻ᷵��NULL
	*/
	inline CCSVField* GetFieldPtr(int nIndex)
	{
		if (nIndex < 0 || nIndex >= m_iFieldCount)
		{
			return NULL;
		}
		return &m_lpFields[nIndex]; /*lint !e676 */
	}
protected:
	/**
	* ͨ�ý����ݴ��ļ��ж���
	* @param nRecordNo��ʼ��¼�ţ���֮�Ӻ���ǰ������
	* @return  0��ʾ�ɹ�������ʧ��
	*/
	int ReadRecord(int iReadRecordNo);
	/**
	* �����ݴ��ļ��ж���
	* @param nRecordNo��ʼ��¼�ţ���ʼ��ȡ��λ�ò������²��ң��ӿ��ٶȣ�
	* @return  0��ʾ�ɹ�������ʧ��
	*/
	int ReadRecordOnIndex(int iReadRecordNo);
private:

	/**
	* ���ֲ���
	* @param a      FieldName2Index���������ַ
	* @param length �ֶζ������鳤��
	* @param key    �����ҵ��ֶ���
	* @param uIndex  �μ�return������ΪNULL
	* @param iType	�Ƿ����ִ�Сд��0�������֣�1������
	* @return true��ʾ�ҵ�����ʱ*uIndexΪ�±꣬false��ʾδ�ҵ�
	*/
	bool BinarySearch(LPCSVFieldName2Index a, size_t length, const char* key, size_t *uIndex, int iType = 0);
	int GetFieldNames();
	void ClearBuffer();

	void CreateBuffer(int iBatchSize);
	int SplitLineBuffer();

	void LineBuffer2Names(int nFieldCount);
	int LineBuffer2Fields(int nFieldCount, int nReadNoInBatch);
private:

	FILE*		m_lpFile;		/**< �ļ�ָ�� */
	uint32_t    m_nRecordCount;/*��¼������*/
	int32_t		m_iFieldCount;		/**< �ļ��ֶθ��� */


	enum AccessFlags
	{
		modeBrowse = 0x0000,
		modeAppend = 0x0001,
		modeEdit = 0x0002
	};
	unsigned int	m_nAccessFlags;

	int m_nBatchCount;		   /**< ����С */

	bool		m_bBOF;		/**< �ļ�ͷ */
	bool		m_bEOF;		/**< �ļ�β */
	uint32_t	m_nRecordNo;/**< ��ǰ��¼�� */
	uint32_t	m_nNoInBatch;/**< ��ǰ������ļ�¼�� */
	uint32_t	m_nBufRecNo0; /**< ����ļ�¼ͷ[m_nBufRecNo0, m_nBufRecNo1) */
	uint32_t	m_nBufRecNo1; /**< ����ļ�¼β */
	uint32_t	m_nAppendRecCount;/**< ��Append�ļ�¼�� */

	LPCSVFieldName2Index m_lpFieldName2Index;/**< �ֶ��������õ�����ָ�� */


	char* CurLineFieldsBuffer[CSV_MAX_FIELDS];
	char LineDataBuffer[MAX_RECORD_LEN];
	char WriteDataBuffer[MAX_RECORD_LEN];
public:
	CCSVField	m_lpFields[CSV_MAX_FIELDS];		/**< �ֶ���Ϣ */
};

#endif//_CSV_H_

