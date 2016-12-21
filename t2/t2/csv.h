/*************************************************************
*	Copyright(c) 2014, 恒生电子股份有限公司
*	All rights reserved.
*
*	文件名称：	csv
*	文件标识：  h
*	摘    要：读写CSV格式数据，暂不支持编辑，中间插入等操作
*
*	当前版本：1.0
*	作    者：	zhouwr
*	创建日期：3/24/2014
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
//格式化错误，数字转换成字符串或者字符串转换成数字出错
#define SVE_FORMAT_FAILED			-1
//长度越界或者数字转换成字符串后超过字段长度
#define SVE_LENGTH_OVERFLOW			-2
//参数错误
#define SVE_WRONG_PARAM				-3

#define MAX_RECORD_LEN 20460
#define MAX_FIELD_CASHE_LEN 10240
typedef char uint8_t ;
#pragma pack(1)
/**< CSV文件每字段结构 */
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
	* 设置每次处理的记录条数
	* @param iBatchCount 批大小
	*/
	//   inline void SetBatchSize(int iBatchCount)
	//   {
	//       if (m_lpFile && iBatchCount > 0)
	//           m_nBatchCount = iBatchCount;
	//   }
	//void ClearBuffer();

private:
	FILE*		m_lpFile;		/**< 文件指针 */
	/// 保存csv文件字段的数组
	CSVFIELD m_lpFieldInfos[CSV_MAX_FIELDS];
	/// 记录当前CSV文件的字段数
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
	* 获取列名
	* @return const char*	列名
	*/
	inline const char* GetName() const
	{
		return (const char*)m_FieldInfo.m_sName;
	}

	/**
	* 获取列号
	* @return int		列号
	*/
	inline int GetNo() const
	{
		return m_nNo;
	}
	/**
	* 获取整型列值
	* @return 整型列值
	*/
	inline int32_t AsInt() const
	{
		return atol(m_lpBufs[m_nCurNoInBatch]);
	}

	/**
	* 获取整型列值
	* @return 整型列值
	*/
	/*inline int64_t AsInt64() const
	{
	int64_t ret=0;
	sscanf(m_lpBufs[m_nCurNoInBatch], "%l", &ret);
	return ret;
	}*/

	/**
	* 获取无符号整型列值
	* @return 无符号整型列值
	*/
	inline uint64_t AsUInt64() const
	{
		uint64_t ret = 0;
		sscanf(m_lpBufs[m_nCurNoInBatch], "%lu", &ret);
		return ret;
	}

	/**
	* 获取无符号整型列值
	* @return 无符号整型列值
	*/
	inline uint32_t AsUInt() const
	{
		uint32_t ret = 0;
		sscanf(m_lpBufs[m_nCurNoInBatch], "%u", &ret);
		return ret;
	}
	/**
	* 获取浮点型列值
	* @return 浮点型列值
	*/
	inline double AsFloat() const
	{
		return atof(m_lpBufs[m_nCurNoInBatch]);
	}
	/**
	* 获取字符型列值
	* @return 字符型列值
	*/
	inline char AsChar() const
	{
		return *(m_lpBufs[m_nCurNoInBatch]);
	}

	/**
	* 获取字符串型列值
	* @return 字符串型列值
	*/
	inline const char* AsString() const
	{
		return m_lpBufs[m_nCurNoInBatch];
	}

	/**
	* 设置整型列值
	* @param nValue：要设置的值
	* @param 0表示成功，其他表示失败
	*/
	int SetValue(int32_t nValue);

	/**
	* 设置整型列值
	* @param nValue：要设置的值
	* @param 0表示成功，其他表示失败
	*/
	int SetValue(uint32_t nValue);

	/**
	* 设置整型列值
	* @param nValue：要设置的值
	* @param 0表示成功，其他表示失败
	*/
	//int SetValue(int64_t nValue);

	/**
	* 设置整型列值
	* @param nValue：要设置的值
	* @param 0表示成功，其他表示失败
	*/
	int SetValue(uint64_t nValue);

	/**
	* 设置双精度型列值
	* @param lfValue	要设置的值
	* @param 0表示成功，其他表示失败
	*/
	int SetValue(double lfValue);

	/**
	* 设置列值
	* @param szValue	要设置的值
	* @param 0表示成功，其他表示失败
	*/
	int SetValue(const char *szValue);

	/**
	* 设置列值，字符串不是以0结尾的
	* @param szValue 字符串
	* @param iLen	设置的字符串长度
	* @param 0表示成功，其他表示失败
	*/
	int SetValue(const char *szValue, int iLen);
private:
	void ClearBuffer();

	void CreateBuffer(int iBatchSize);
private:
	int			m_nNo;/**< 列号 */
	CSVFIELD	m_FieldInfo;/**< 列信息 */
	int m_nBatchCount;//
	int m_nCurNoInBatch; //当前批处理缓存
	char** m_lpBufs; //列缓存
};
class CSV
{
public:
	CSV();
	~CSV();
	int OpenFile(const char *fileName, uint32_t nOpenFlags, int iBatchSize = 10);
	void CloseFile();
	/** 移动到文件记录开始
	* @return 无
	*/
	void MoveFirst();

	/** 相对于当前记录，想后移动一条记录
	* @return 无
	*/
	void MoveNext();

	/** 移动到文件记录末尾
	* @return 无
	*/
	void MoveLast();
	/**
	* 是否是文件末尾
	* @return true表示文件末尾，反之false
	*/
	inline bool IsEOF()
	{
		return m_bEOF;
	}
	/** 添加一条或多条记录
	* @param nAppendRecCount 要添加的记录数
	* @return true 成功
	*/
	bool Append(int appendRecCount = 1);
	/** 添加下一条记录
	* @return true 成功，false 失败
	*/
	bool AppendNext();

	/** 提交修改或添加的记录
	* @return 0表示成功，其他失败（写文件）
	*/
	int Post();

	/**
	* 获取字段值引用,会调用GetField(int nIndex)函数。
	* @param szFieldName	字段名
	* @return 字段名对应的字段值指针
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
	* 获取字段值的指针
	* @param nIndex	字段下标
	* @return 字段名对应的字段值指针，如果失败会返回NULL
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
	* 通用将数据从文件中读出
	* @param nRecordNo起始记录号，反之从后往前读数据
	* @return  0表示成功，否则失败
	*/
	int ReadRecord(int iReadRecordNo);
	/**
	* 将数据从文件中读出
	* @param nRecordNo起始记录号，开始读取的位置不再重新查找，加快速度，
	* @return  0表示成功，否则失败
	*/
	int ReadRecordOnIndex(int iReadRecordNo);
private:

	/**
	* 二分查找
	* @param a      FieldName2Index对象数组地址
	* @param length 字段对象数组长度
	* @param key    欲查找的字段名
	* @param uIndex  参见return，可以为NULL
	* @param iType	是否区分大小写，0：不区分，1：区分
	* @return true表示找到，此时*uIndex为下标，false表示未找到
	*/
	bool BinarySearch(LPCSVFieldName2Index a, size_t length, const char* key, size_t *uIndex, int iType = 0);
	int GetFieldNames();
	void ClearBuffer();

	void CreateBuffer(int iBatchSize);
	int SplitLineBuffer();

	void LineBuffer2Names(int nFieldCount);
	int LineBuffer2Fields(int nFieldCount, int nReadNoInBatch);
private:

	FILE*		m_lpFile;		/**< 文件指针 */
	uint32_t    m_nRecordCount;/*记录总条数*/
	int32_t		m_iFieldCount;		/**< 文件字段个数 */


	enum AccessFlags
	{
		modeBrowse = 0x0000,
		modeAppend = 0x0001,
		modeEdit = 0x0002
	};
	unsigned int	m_nAccessFlags;

	int m_nBatchCount;		   /**< 批大小 */

	bool		m_bBOF;		/**< 文件头 */
	bool		m_bEOF;		/**< 文件尾 */
	uint32_t	m_nRecordNo;/**< 当前记录号 */
	uint32_t	m_nNoInBatch;/**< 当前批处理的记录号 */
	uint32_t	m_nBufRecNo0; /**< 缓存的记录头[m_nBufRecNo0, m_nBufRecNo1) */
	uint32_t	m_nBufRecNo1; /**< 缓存的记录尾 */
	uint32_t	m_nAppendRecCount;/**< 欲Append的记录数 */

	LPCSVFieldName2Index m_lpFieldName2Index;/**< 字段名排序用的数组指针 */


	char* CurLineFieldsBuffer[CSV_MAX_FIELDS];
	char LineDataBuffer[MAX_RECORD_LEN];
	char WriteDataBuffer[MAX_RECORD_LEN];
public:
	CCSVField	m_lpFields[CSV_MAX_FIELDS];		/**< 字段信息 */
};

#endif//_CSV_H_

