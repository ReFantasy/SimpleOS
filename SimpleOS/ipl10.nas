; SimpleOS引导扇区代码
; TAB=4

CYLS	EQU		10				; 读取磁盘柱面的个数

		ORG		0x7c00			; 程序的载入地址

; FAT12格式数据

		JMP		entry
		DB		0x90
		DB		"HARIBOTE"		; 启动区的名称
		DW		512				; 每个扇区的大小
		DB		1				; 簇的大小
		DW		1				; Boot记录占用多少扇区
		DB		2				; 共有多少FAT表（必须为2）
		DW		224				; 根目录的大小（一般设成224）
		DW		2880			; 扇区总数（必须2880扇区）
		DB		0xf0			; 磁盘的种类（必须0xf0）
		DW		9				; FAT的长度（必须9扇区）
		DW		18				; 每磁道扇区数（必须是18）
		DW		2				; 磁头数（面数）
		DD		0				; 不使用分区，必须0
		DD		2880			; 重写一次磁盘大小
		DB		0,0,0x29		; 
		DD		0xffffffff		; 
		DB		"HARIBOTEOS "	; 磁盘名称（11字节）
		DB		"FAT12   "		; 文件系统类型（8字节）
		RESB	18				; 随便空出18字节

; 启动区程序入口

entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

; 读磁盘
		;设置参数
		MOV		AX,0x0820		;磁盘数据读读入的内存地址ES:BX
		MOV		ES,AX
		MOV		CH,0			; 柱面0
		MOV		DH,0			; 磁头0
		MOV		CL,2			; 从第二个扇区开始读入（第一个扇区已经被CPU自动加载）
readloop:	
		MOV		SI,0			;记录读取失败的次数
retry:		
		;开始读取
		MOV		AH,0x02			; AH=0x02 : 读取磁盘中断设置
		MOV		AL,1			; 读取的扇区个数1
		MOV		BX,0
		MOV		DL,0x00			; 指定从哪个驱动器的软盘上读取数据
		INT		0x13			; 调用读磁盘中断
		JNC		next			; 读取没出错跳转到next
		
		ADD		SI,1			;读取错误，SI+1
		CMP		SI,5
		JAE		error			;读取错误大于等于5次，跳转到error
		
		MOV		AH,0x00			;复位软盘驱动器
		MOV		DL,0x00
		INT		0x13
		JMP		retry
next:
		MOV		AX,ES			;内存地址后移一个扇区的大小（512字节）
		ADD		AX,0x0020
		MOV		ES,AX
		ADD		CL,1			;扇区编号+1
		CMP		CL,18			;是否读完18个扇区
		JBE		readloop		;如果CL<=18 跳转到readloop
		;开始读取反面
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readloop
		;开始读取下一个柱面
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB		readloop
		
		MOV		[0x0ff0],CH		;保存读取的柱面数 
		JMP		0xc200			;跳转到操作系统代码


;错误信息

error:
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI+1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示一个字符
		MOV		BX,15			; 指定字符颜色
		INT		0x10			; 调用显卡BIOS中断
		JMP		putloop

fin:
		HLT						; 暂停CPU，直到有点击或者按键等按下时唤醒
		JMP		fin				; 无限循环
msg:
		DB		0x0a, 0x0a		; 2个换行
		DB		"load error"
		DB		0x0a			; 换行
		DB		0

		RESB	0x7dfe-$		; 填写0x00，直到0x001fe

		DB		0x55, 0xaa		;第一个扇区最后两个字节 必须是0x55, 0xaa 表示存在操作系统
