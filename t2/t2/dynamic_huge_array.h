/** @file
* ��ź������������2^32-1�飩������Ķ�ά���飬�ֿ������ڴ棬��������Ϊһά���顣
*ֻ����һ��д�������ж�������������ڲ�����gcc���õ��ڴ����Ϻ�����
*
* @author  ���پ�
* @author  
* @version 1.0
* @date    20130706 ��ʼ�汾
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
		// ��ų���Χ
		if (nPiece >= PIECE_COUNT)
		{
			return (uint32)-1;
		}

		// ����ָ�����֮ǰ������Piece
		for (uint32 i = m_nPieceCount; i <= nPiece; ++i, ++m_nPieceCount)
		{
			//������Piece 
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
	* ˳���������������ߵ��ã�����Add�Ĳ��ܵ���Set��
	* @param data ������
	* @return �����±꣬-1��ʾʧ��
	*/
	uint32 Add(const T &data)
	{
		ITEM *lpItem;
		
		// ��ǰPieceδ��
		if (m_nIndexInPiece < ITEMS_PER_PIECE)
		{
			lpItem = &m_Pieces[m_nPieceCount - 1][m_nIndexInPiece];
		}
		else if (m_nPieceCount < PIECE_COUNT) // ��ǰPiece����������Piece
		{
			// ������Piece 
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

		// ��֤lpItem->Data����ֵ�������Flag
		MEM_BARRIER;

		lpItem->Flag = 1;
		
		m_nIndexInPiece++;
		m_nMaxIndex = m_nCount;

		return m_nCount++;
	}

	/**
	* ����ָ����ŵ�����������ߵ��ã�����Set�Ĳ��ܵ���Add��
	* @param nIndex ���
	* @param data ������
	* @return �����±꣬-1��ʾʧ��
	*/
	uint32 Set(uint32 nIndex, const T &data)
	{
		ITEM *lpItem;

		uint32 nPiece = GetPiece(nIndex);
		// ��ų���Χ
		if (nPiece >= PIECE_COUNT)
		{
			return (uint32)-1;
		}

		// ����ָ�����֮ǰ������Piece
		for (uint32 i = m_nPieceCount; i <= nPiece; ++i, ++m_nPieceCount)
		{
			//������Piece 
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

		// ��֤lpItem->Data����ֵ�������Flag
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
	* ȡָ����ŵ�������
	* @param nIndex ���
	* @return ������ָ��
	*/
	T * Get(uint32 nIndex)
	{
		// ����ֻ�ж�nIndex <= m_nMaxIndex����m_nMaxIndex=0�����
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
	///ÿ��Piece������
	static const uint32 ITEMS_PER_PIECE = (1 << SHIFT_COUNT);
	///���Piece����������һ����Ϊ�˱������Ϊ-1
	static const uint32 PIECE_COUNT = ((1 << (32 - SHIFT_COUNT)) - 1);
	///���ڼ���Piece�е���ŵ�����
	static const uint32 INDEX_MASK = (ITEMS_PER_PIECE - 1);

	///ȡPiece��
	inline uint32 GetPiece(uint32 nIndex)
	{
		return nIndex >> SHIFT_COUNT;
	}
	///ȡPiece�е����
	inline uint32 GetIndex(uint32 nIndex)
	{
		return nIndex & INDEX_MASK;
	}

	typedef struct
	{
		///����
		T					Data;
		///��־λ����0λ������ռ�ã�1~31��Ӧÿ�������ߵ�ȷ��
		volatile uint32	Flag;
	} ITEM;

	///��ǰPiece��
	uint32			m_nPieceCount;
	///��ǰPiece������
	uint32			m_nIndexInPiece;
	///����������
	uint32			m_nCount;
	///�������������
	uint32			m_nMaxIndex;
	///Piece����
	ITEM *				m_Pieces[PIECE_COUNT];
};

#endif // _DYNAMIC_HUGE_ARRAY_H_
