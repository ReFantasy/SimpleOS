/* 键盘中断处理函数 */

#include "bootpack.h"

struct FIFO8 keyfifo;  //定义键盘数据缓冲

void inthandler21(int *esp)  //PS/2键盘的中断
{
	unsigned char data;
	io_out8(PIC0_OCW2, 0x61);	/* 通知PIC IRQ-01已经受理完毕 */
	data = io_in8(PORT_KEYDAT);
	
	fifo8_put(&keyfifo,data);

	return;
}

#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

void wait_KBC_sendready(void)
{
	/* 等待键盘控制电路准备完毕 */
	while(1) 
	{
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) break;
	}
	return;
}

void init_keyboard(void)
{
	/* 初始化键盘控制电路 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}
