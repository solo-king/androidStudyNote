参考资料:
    https://www.ibm.com/developerworks/rational/library/inline-assembly-c-cpp-guide/index.html
    //较好的参考
    https://zhuanlan.zhihu.com/p/28917153


需要指出的是，只有部分指令用到了指令模板中的所有域。模板中各字段的作用如下所示：
MNEMONIC - 指令的助记符如ADD
{S} - 可选的扩展位
- 如果指令后加了S，将依据计算结果更新CPSR寄存器中相应的FLAG
{condition} - 执行条件，如果没有指定，默认为AL(无条件执行)
{Rd} - 目的寄存器，存储指令计算结果
Operand1 - 第一个操作数，可以是一个寄存器或一个立即数
Operand2 - 第二个(可变)操作数
- 可以是一个立即数或寄存器甚至带移位操作的寄存器