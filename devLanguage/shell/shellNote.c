                                                shell学习
local:
    一般用于局部变量声明，多在在函数内部使用。
    shell脚本中定义的变量是global的，其作用域从被定义的地方开始，到shell结束或被显示删除的地方为止。
    shell函数定义的变量默认是global的，其作用域从“函数被调用时执行变量定义的地方”开始，到shell结束或被显示删除处为止。函数定义的变量可以被显示定义成local的，其作用域局限于函数内。但请注意，函数的参数是local的。
    如果同名，Shell函数定义的local变量会屏蔽脚本定义的global变量。

    使用举例：
    #!/bin/bash
    function Hello()
    {
            local text="Hello World!!!" #局部变量
            echo $text
    }
    输出:
        Hello
数组方式调用:
    #!/bin/bash
    ARRARY_LIST=(ganye ganye1 ganye2 ganye3)

    for tmp in ${ARRARY_LIST[@]}
    do
        echo $tmp
    done
在if中添加x前缀
    if [ "x$TARGET_PRODUCT" != x ] ; then
    防止出现空的情况
if高级特性:
    1.双括号命令允许你在比较过程中使用高级数学表达式
        if (( $val1 ** 2 > 90 ))
    2.双方括号命令提供了针对字符串比较的高级特性
        if [[ $USER == r* ]]
特殊变量含义:
    $#
        特殊变量$#含有脚本运行时携带的命令行参数的个数
        只统计参数的个数
    $*
        变量会将命令行上提供的所有参数当作一个单词保存。这个单词包含了命令行中出现的每
        一个参数值。基本上$*变量会将这些参数视为一个整体，而不是多个个体。
    $@
        变量会将命令行上提供的所有参数当作同一字符串中的多个独立的单词。这样
        你就能够遍历所有的参数值，得到每个参数。这通常通过for命令完成。
判断运算符:
    Note:
        如下内容来自<Linux 命令行与shell脚本变成大全第3版> P238
    数值比较:
        n1 -eq n2 检查n1是否与n2相等
        n1 -ge n2 检查n1是否大于或等于n2
        n1 -gt n2 检查n1是否大于n2
        n1 -le n2 检查n1是否小于或等于n2
        n1 -lt n2 检查n1是否小于n2
        n1 -ne n2 检查n1是否不等于n2
    字符串比较:
        =  检查是否相同
        !=
        <  判断字符串大小，按字典顺序
        >  判断字符串大小，按字典顺序
        -n 检查字符串的长度是否为非零
        -z 检查字符串的长度是否为零
    文件比较:   
        -d file 检查file是否存在
        -e file 检查file是否存在
        -f file 检查是否存在一个file并且其是否为文件类型
        -r file 检查是否可读
        -s file 检查file是否存在并非空
        -w file 
        -x
        -O file 检查file是否存在并属于当前用户所有
        -G file 检查file是否存在并且默认组与当前用户相同
        file1 -nt file2 检查file1是否比file2新
        file1 -ot file2 检查file1是否比file2旧
    条件并列:
        [] && []
        [] || []

