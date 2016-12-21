#include "memcheck.h"

// ȡ��malloc, calloc, free�ĺ궨��
#undef malloc
#undef calloc
#undef free



/**
 * ��������ڵ㣬��ʾһ���ڴ�й©��Ϣ
 */
typedef struct _mem_node
{
	void *ptr;		// й©�ڴ��ַ
	size_t block;	// й©�ڴ��С
	size_t line;	// й¶�����Ĵ�����
	char *filename;	// й©�������ļ���
	struct _mem_node *next;	// ��һ���ڵ�ָ��
} mem_node;

// ����ָ��ͷ�ڵ��ָ��
mem_node *head = NULL;

/**
 * ����һ���ڵ㲢��������
 * @param ptr ������ڴ��ַ
 * @param block ������ڴ浥Ԫ��С
 * @param line �����к�
 * @param filename �ļ�����
 */
static void mem_node_add(void *ptr, size_t block, size_t line, char *filename)
{
	// �����ڵ�
	mem_node *node = malloc(sizeof(mem_node));
	node->ptr = ptr;
	node->block = block;
	node->line = line;
	node->filename = filename;
	node->next = NULL;

	// ��������ͷ�ڵ�
	if (head)
	{
		node->next = head;
		head = node;
	}
	else
		head = node;
}


/**
 * ��������ɾ��һ���ڵ�
 * @param ptr ������ڴ��ַ
 */
static void mem_node_remove(void *ptr)
{
	// �ж�ͷ�ڵ��Ƿ����
	if (head)
	{
		// ����ͷ�ڵ�
		if (head->ptr == ptr)
		{
			// ��ȡͷ�ڵ����һ���ڵ�
			mem_node *pn = head->next;
			// ɾ��ͷ�ڵ�
			free(head);
			// ��ͷ�ڵ�ָ��ָ����һ���ڵ�
			head = pn;
		}
		else	// �ж������Ƿ�Ϊ��
		{
			// ָ��ڵ��ָ��
			mem_node *pn = head->next;
			// ָ��ǰһ���ڵ��ָ��
			mem_node *pc = head;
			// �������нڵ�
			while (pn)
			{
				// ��ȡָ����һ���ڵ��ָ��
				mem_node *pnext = pn->next;
				if (pn->ptr == ptr)
				{
					pc->next = pnext;	// ɾ����ǰ�ڵ�
					free(pn);
				}
				else
					pc = pc->next;
				pn = pnext;
			}
		}
	}
}

/**
 * ��ʾ�ڴ�й©��Ϣ
 */
void show_block()
{
	if (head)
	{
		// �������ڴ�й©����
		size_t total = 0;
		// ָ��ͷ�ڵ��ָ��
		mem_node *pn = head;

		// �������
		puts("\n\n-------------------------------�ڴ�й©����------------------------------------\n");

		// ��������
		while (pn)
		{
			mem_node *pnext = pn->next;
			// �����ļ���
			char *pfile = pn->filename, *plast = pn->filename;
			while (*pfile)
			{
				// �ҵ�\�ַ�
				if (*pfile == '\\')
					plast = pfile + 1;	// ��ȡ\�ַ���λ��
				pfile++;
			}
			// ����ڴ�й©��Ϣ
			printf("λ��:%s(%d), ��ַ:%p(%dbyte)\n", plast, pn->line, pn->ptr, pn->block);
			// �ۼ��ڴ�й©����
			total += pn->block;
			// ɾ������ڵ�
			free(pn);
			// ָ����һ���ڵ�
			pn = pnext;
		}
		printf("�ܼ��ڴ�й©:%dbyte\n", total);
	}
}

/**
 * ���ڵ��Ե�malloc����
 * @param elem_size �����ڴ��С
 * @param filename �ļ�����
 * @param line �����к�
 */
void *dbg_malloc1(size_t elem_size, char *filename, size_t line)
{
	void *ptr = malloc(elem_size);
	// �������ڴ�ĵ�ַ��������
	mem_node_add(ptr, elem_size, line, filename);
	return ptr;
}

/**
 * ���ڵ��Ե�calloc����
 * @param count �����ڴ浥Ԫ����
 * @param elem_size ÿ��Ԫ�ڴ��С
 * @param filename �ļ�����
 * @param line �����к�
 */
void *dbg_calloc1(size_t count, size_t elem_size, char *filename, size_t line)
{
	void *ptr = calloc(count, elem_size);
	// �������ڴ�ĵ�ַ��������
	mem_node_add(ptr, elem_size * count, line, filename);
	return ptr;
}

/**
 * ���ڵ��Ե�free����
 * @param ptr Ҫ�ͷŵ��ڴ��ַ
 */
void dbg_free1(void *ptr)
{
	free(ptr);
	// ��������ɾ���ڵ�
	mem_node_remove(ptr);
}
