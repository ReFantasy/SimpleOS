/*
*                  图形显示
*/

#include "bootpack.h"


void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = 
	{
		0x00, 0x00, 0x00,	/*  0:榛戣壊 */
		0xff, 0x00, 0x00,	/*  1:浜?*/
		0x00, 0xff, 0x00,	/*  2:浜?*/
		0xff, 0xff, 0x00,	/*  3:浜粍 */
		0x00, 0x00, 0xff,	/*  4:浜?*/
		0xff, 0x00, 0xff,	/*  5:浜传 */
		0x00, 0xff, 0xff,	/*  6:娴呬寒? */
		0xff, 0xff, 0xff,	/*  7:鐧?*/
		0xc6, 0xc6, 0xc6,	/*  8:浜伆 */
		0x84, 0x00, 0x00,	/*  9:鏆?*/
		0x00, 0x84, 0x00,	/* 10:鏆?*/
		0x84, 0x84, 0x00,	/* 11:鏆楅粍 */
		0x00, 0x00, 0x84,	/* 12:鏆楅潚*/
		0x84, 0x00, 0x84,	/* 13:鏆楃传 */
		0x00, 0x84, 0x84,	/* 14:娴呮殫? */
		0x84, 0x84, 0x84	/* 15:鏆楃伆 */
	};
	set_palette(0, 15, table_rgb);
	return;

}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* 读取中断标志*/
	io_cli(); 					/* 关闭中断*/
	io_out8(0x03c8, start);  		
	for (i = start; i <= end; i++) 
	{
		io_out8(0x03c9, rgb[0] / 4);//R
		io_out8(0x03c9, rgb[1] / 4);//G
		io_out8(0x03c9, rgb[2] / 4);//B
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 打开中断 */
	return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char color, int x1, int y1, int x2, int y2)
{
	int x = 0, y = 0;
	
	for (y = y1; y <= y2; y++) 
	{
		for (x = x1; x <= x2; x++)
			vram[y * xsize + x] = color;
	}
	
	return;
}

void init_screen8(char *vram, int x, int y)
{
	boxfill8(vram, x, COL8_008484,  0,     0,      x -  1, y - 29);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 26, x -  1, y -  1);

	boxfill8(vram, x, COL8_FFFFFF,  3,     y - 24, 59,     y - 24);
	boxfill8(vram, x, COL8_FFFFFF,  2,     y - 24,  2,     y -  4);
	boxfill8(vram, x, COL8_848484,  3,     y -  4, 59,     y -  4);
	boxfill8(vram, x, COL8_848484, 59,     y - 23, 59,     y -  5);
	boxfill8(vram, x, COL8_000000,  2,     y -  3, 59,     y -  3);
	boxfill8(vram, x, COL8_000000, 60,     y - 24, 60,     y -  3);

	boxfill8(vram, x, COL8_848484, x - 47, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y -  4);
	boxfill8(vram, x, COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);
	return;
}

void putfont8(char *vram, int xsize, int x, int y, char font_color, char *character_addr)
{
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) 
	{
		p = vram + (y + i) * xsize + x;
		d = character_addr[i];  //每个字符一个字节
		if ((d & 0x80) != 0) { p[0] = font_color; }  //读取每字节的8个位状态
		if ((d & 0x40) != 0) { p[1] = font_color; }
		if ((d & 0x20) != 0) { p[2] = font_color; }
		if ((d & 0x10) != 0) { p[3] = font_color; }
		if ((d & 0x08) != 0) { p[4] = font_color; }
		if ((d & 0x04) != 0) { p[5] = font_color; }
		if ((d & 0x02) != 0) { p[6] = font_color; }
		if ((d & 0x01) != 0) { p[7] = font_color; }
	}
	return;
}
void putfonts8_asc(char *vram, int xsize, int x, int y, char font_color, unsigned char *str)  //显示字符串
{
	extern char hankaku[4096];
	
	int i = 0;
	for(i = 0;str[i] != 0x00; i++)
	{
		putfont8(vram, xsize, x+i*8, y, font_color, hankaku+str[i]*16);
	}
	
	// for(;*str != 0x00; str++)
	// {
		// putfont8(vram,xsize,x,y,font_color,hankaku+(*str)*16);
		// x += 8;
	// }
	return;
}


void init_mouse_cursor8(char *mouse, char bc)//初始化鼠标图标16x16
{
	static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;

	for (y = 0; y < 16; y++) 
	{
		for (x = 0; x < 16; x++) 
		{
			if (cursor[y][x] == '*') 
			{
				mouse[y * 16 + x] = COL8_000000;
			}
			if (cursor[y][x] == 'O') 
			{
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') 
			{
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return;
}

void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize)
{
	int x, y;
	for (y = 0; y < pysize; y++) 
	{
		for (x = 0; x < pxsize; x++) 
		{
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}

