/** @file
* 存放海量（最大容量2^32-1块）数据项的二维数组，分块申请内存，对外体现为一维数组。
*只能有一个写，可以有多个读，本对象内部采用gcc内置的内存屏障函数。
*
* @author  何仲君
* @author  
* @version 1.0
* @date    20130706 初始版本
*/

#ifndef _DYNAMIC_HUGE_ARRAY_H_
#define _DYNAMIC_HUGE_ARRAY_H_

//#include <os/config_env.h>
//#include <os/inttypes.h>

#ifdef _WIN32
//#include <intrin.h>
//#pragma intrinsic(_ReadWriteBarrier)
//#define MEM_BARRIER		_ReadWriteBarrier()
#define MEM_BARRIER		MemoryBarrier()
#else
//gcc
#define MEM_BARRIER		__sync_synchronize()

#endif

typedef unsigned int		uint32;
typedef unsigned char		uint8;

template <class T, uint8 SHIFT_COUNT = 16> class CRM3DynamicHugeArray
{
public:	
	CRM3DynamicHugeArray()
	{
		assert(SHIFT_COUNT < 31);

		m_nPieceCount = 0;
		m_nIndexInPiece = ITEMS_PER_PIECE;
		m_nCount = 0;
		m_nMaxIndex = 0;
		memset(m_Pieces, 0, sizeof(m_Pieces));
	}
	
	virtual ~CRM3DynamicHugeArray()
	{
		for (uint32 i = 0; i < m_nPieceCount; ++i)
		{
			free(m_Pieces[i]);
		}
	}
	
	uint32 SetInitialIndex(uint32 nIndex)
	{
		ITEM *lpItem;

		if (nIndex == 0 || nIndex <= m_nMaxIndex + 1)
		{
			return 0;
		}

		uint32 nPiece = GetPiece(nIndex);
		// 序号超范围
		if (nPiece >= PIECE_COUNT)
		{
			return (uint32)-1;
		}

		// 申请指定序号之前的所有Piece
		for (uint32 i = m_nPieceCount; i <= nPiece; ++i, ++m_nPieceCount)
		{
			//申请新Piece 
			lpItem = (ITEM *)calloc(ITEMS_PER_PIECE, sizeof(ITEM));
			if (lpItem != NULL)
			{
				m_Pieces[i] = lpItem;
			}
			else
			{
				//perror("calloc");
				return (uint32)-1;
			}
		}

		for (uint32 i = m_nMaxIndex + 1; i < nIndex; ++i)
		{
			m_Pieces[GetPiece(i)][GetIndex(i)].Flag = 1;
		}

		m_nIndexInPiece = GetIndex(nIndex);
		m_nMaxIndex = nIndex - 1;
		m_nCount = nIndex;

		return nIndex;
	}
	/**
	* 顺序添加数据项。发送者调用，调用Add的不能调用Set。
	* @param data 数据项
	* @return 返回下标，-1表示失败
	*/
	uint32 Add(const T &data)
	{
		ITEM *lpItem;
		
		// 当前Piece未满
		if (m_nIndexInPiece < ITEMS_PER_PIECE)
		{
			lpItem = &m_Pieces[m_nPieceCount - 1][m_nIndexInPiece];
		}
		else if (m_nPieceCount < PIECE_COUNT) // 当前Piece已满，增加Piece
		{
			// 申请新Piece 
			lpItem = (ITEM *)calloc(ITEMS_PER_PIECE, sizeof(ITEM));
			if (lpItem != NULL)
			{
				m_Pieces[m_nPieceCount++] = lpItem;
				m_nIndexInPiece = 0;
			}
			else
			{
				//perror("calloc");
				return (uint32)-1;
			}
		}
		else
		{
			return (uint32)-1;
		}

		//printf("Add: %d, %d\n", m_nPieceCount - 1, m_nIndexInPiece);
		
		lpItem->Data = data;

		// 保证lpItem->Data被赋值后才设置Flag
		MEM_BARRIER;

		lpItem->Flag = 1;
		
		m_nIndexInPiece++;
		m_nMaxIndex = m_nCount;

		return m_nCount++;
	}

	/**
	* 设置指定序号的数据项。接收者调用，调用Set的不能调用Add。
	* @param nIndex 序号
	* @param data 数据项
	* @return 返回下标，-1表示失败
	*/
	uint32 Set(uint32 nIndex, const T &data)
	{
		ITEM *lpItem;

		uint32 nPiece = GetPiece(nIndex);
		// 序号超范围
		if (nPiece >= PIECE_COUNT)
		{
			return (uint32)-1;
		}

		// 申请指定序号之前的所有Piece
		for (uint32 i = m_nPieceCount; i <= nPiece; ++i, ++m_nPieceCount)
		{
			//申请新Piece 
			lpItem = (ITEM *)calloc(ITEMS_PER_PIECE, sizeof(ITEM));
			if (lpItem != NULL)
			{
				m_Pieces[i] = lpItem;
			}
			else
			{
				//perror("calloc");
				return (uint32)-1;
			}
		}
		lpItem = &m_Pieces[nPiece][GetIndex(nIndex)];

		//assert(lpItem->Flag == 0);

		lpItem->Data = data;

		// 保证lpItem->Data被赋值后才设置Flag
		MEM_BARRIER;
		lpItem->Flag = 1;

		m_nCount++;
		if (nIndex > m_nMaxIndex)
		{
			m_nMaxIndex = nIndex;
		}

		return nIndex;
	}

	/**
	* 取指定序号的数据项
	* @param nIndex 序号
	* @return 数据项指针
	*/
	T * Get(uint32 nIndex)
	{
		// 不能只判断nIndex <= m_nMaxIndex，有m_nMaxIndex=0的情况
		uint32 nPiece = GetPiece(nIndex);
		if (nPiece < m_nPieceCount && nIndex <= m_nMaxIndex)
		{
			ITEM *lpItem = &m_Pieces[nPiece][GetIndex(nIndex)];
			if (lpItem->Flag != 0)
			{
				return &lpItem->Data;
			}
		}

		return NULL;
	}

	uint32 GetCount()
	{
		return m_nCount;
	}

	uint32 GetMaxIndex()
	{
		return m_nMaxIndex;
	}

private:
	///每个Piece的项数
	static const uint32 ITEMS_PER_PIECE = (1 << SHIFT_COUNT);
	///最多Piece数。故意少一，是为了避免序号为-1
	static const uint32 PIECE_COUNT = ((1 << (32 - SHIFT_COUNT)) - 1);
	///用于计算Piece中的序号的掩码
	static const uint32 INDEX_MASK = (ITEMS_PER_PIECE - 1);

	///取Piece号
	inline uint32 GetPiece(uint32 nIndex)
	{
		return nIndex >> SHIFT_COUNT;
	}
	///取Piece中的序号
	inline uint32 GetIndex(uint32 nIndex)
	{
		return nIndex & INDEX_MASK;
	}

	typedef struct
	{
		///数据
		T					Data;
		///标志位，第0位发送者占用，1~31对应每个接收者的确认
		volatile uint32	Flag;
	} ITEM;

	///当前Piece数
	uint32			m_nPieceCount;
	///当前Piece中已用
	uint32			m_nIndexInPiece;
	///总数据项数
	uint32			m_nCount;
	///数据项的最大序号
	uint32			m_nMaxIndex;
	///Piece数组
	ITEM *				m_Pieces[PIECE_COUNT];
};

#endif // _DYNAMIC_HUGE_ARRAY_H_
