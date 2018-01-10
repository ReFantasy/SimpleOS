# SimpleOS

读取的柱面个数保存地址 [0x0ff0]

16位寄存器:

- AX accumulator,累加寄存器
- CX counter,计数寄存器
- DX data,数据寄存器
- BX base,基址寄存器
- SP stack pointer,栈指针寄存器
- BP base pointer,基址指针寄存器
- SI source index,源变址寄存器
- DI destination index,目的变址寄存器

8位寄存器:

- AL 累加寄存器低位
- CL 计数寄存器低位
- DL 数据寄存器低位
- BL 基址寄存器低位
- AH 累加寄存器高位
- CH 计数寄存器高位
- DH 数据寄存器高位
- BH 基址寄存器高位

32位寄存器:

- EAX accumulator,累加寄存器
- ECX counter,计数寄存器
- EDX data,数据寄存器
- EBX base,基址寄存器
- ESP stack pointer,栈指针寄存器
- EBP base pointer,基址指针寄存器
- ESI source index,源变址寄存器
- EDI destination index,目的变址寄存器

段寄存器：

- ES 附加段寄存器
- CS 代码段寄存器
- SS 栈段寄存器
- DS 数据段寄存器
- FS 没有名称
- GS 没有名称

- JMP 跳转指令
- CMP 比较指令
- JE 条件跳转
```
// CMP AL,0
// JE fin
if(AL == 0)
{
    goto fin;
}
```