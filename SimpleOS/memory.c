/* 内存管理 */

#include "bootpack.h"

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

unsigned int memtest(unsigned int start, unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;  //eflag第18位表示CPU版本 cr0寄存器用于禁止CPU高速缓存

	// CPU是386还是486以上的
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; // AC-bit = 1
	io_store_eflags(eflg);
	eflg = io_load_eflags();

	if ((eflg & EFLAGS_AC_BIT) != 0)
	{
		flg486 = 1;  //如果是386 即使设定AC=1，AC的也会自动回到0
	}
	eflg &= ~EFLAGS_AC_BIT; // AC-bit = 0
	io_store_eflags(eflg);

	if (flg486 != 0)
	{
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; // 禁止高速缓存
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0)
	{
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; 
		store_cr0(cr0);
	}

	return i;
}

void memman_init(struct MEMMAN *man)
{
	man->frees = 0;			/* 可用内存段个数 */
	man->maxfrees = 0;		/* 用于观察可用状况：frees的最大值 */
	man->lostsize = 0;		/*释放失败的内存的大小总和 */
	man->losts = 0;			/* 释放失败的次数 */
	return;
}

//剩余可用内存总大小
unsigned int memman_total(struct MEMMAN *man)
{
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++)
	{
		t += man->free[i].size;
	}
	return t;
}

//内存申请
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
	unsigned int i, a;
	for (i = 0; i < man->frees; i++)
	{
		if (man->free[i].size >= size)
		{
			/* 找到可分配的内存 */
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0)  //该段的剩余内存已空
			{
				man->frees--;
				for (; i < man->frees; i++)
				{
					man->free[i] = man->free[i + 1]; 
				}
			}
			return a;
		}
	}
	return 0; /* 没有可用空间 */
}

//释放内存，free[]按照地址的大小顺序排列
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i, j;
	//找到释放内存的插入位置
	for (i = 0; i < man->frees; i++)
	{
		if (man->free[i].addr > addr)
		{
			break;
		}
	}
	
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0)
	{
		if (man->free[i - 1].addr + man->free[i - 1].size == addr)  //是否可接到前面
		{
			man->free[i - 1].size += size;
			if (i < man->frees)
			{
				if (addr + size == man->free[i].addr)//是否可接到后面
				{
					man->free[i - 1].size += man->free[i].size;
					man->frees--;
					for (; i < man->frees; i++)
					{
						man->free[i] = man->free[i + 1]; 
					}
				}
			}
			return 0; /* 成功終了 */
		}
	}
	/* 不能与前面的接到一起 */
	if (i < man->frees)
	{
		if (addr + size == man->free[i].addr)
		{
			
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* 成功終了 */
		}
	}
	/* 既不能与前面接到一起，也不能与后面接到一起 */
	if (man->frees < MEMMAN_FREES) //判断是否还有可用的数组元素保存内内存信息
	{
		/* free[i]之后的往后移 空出位置保存此次释放的内存信息 */
		for (j = man->frees; j > i; j--)
		{
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees)
		{
			man->maxfrees = man->frees; /* 最大値を更新 */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* 成功終了 */
	}
	/* 内存丢失 */
	man->losts++;
	man->lostsize += size;
	return -1; /* 失敗終了 */
}

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size)
{
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;
	a = memman_alloc(man, size);
	return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}
