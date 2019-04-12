make 选项:
    -n:
        只打印命令执行结果，不打印执行命令
    -M  
        Instead of outputting the result of preprocessing, output a rule suitable for make describing the dependencies of the main source file.
        输出生成目标文件的依赖关系，并非输出预处理的信息
函数:
    wildcard:
        它有一个参数，功能是展开成一列所有符合由其参数描述的文 件名，文件间以空格间隔
        SOURCES = $(wildcard *.c)
    patsubst:
        它需要３个参数——第一个是一个需要匹配的 式样，第二个表示用什么来替换它，第三个是一个需要被处理的由空格分隔的字列。例如，处理那个经过上面定义后的变量
        OBJS = $(patsubst %.c,%.o,$(SOURCES))
    subst:
        这是一个替换函数，这个函数有三个参数，第一个参数是被替换字串，第二个参数是替换字串，第三个参数是替换操作作用的字串。
        $(subst <from>,<to>,<text>)
        名称：字符串替换函数——subst。
        功能：把字串<text>中的<from>字符串替换成<to>。
        返回：函数返回被替换过后的字符串。
        示例：
        $(subst ee,EE,feet on the street)，
        把“feet on the street”中的“ee”替换成“EE”，返回结果是“fEEt on the strEEt”。
    sort:
    call:
        call 函数是唯一一个可以用来创建新的参数化的函数。你可以写一个非常复杂的表达式，这个表达式中，
        你可以定义许多参数，然后你可以用 call 函数来向这个表达式传递参数。其语法是：
            $(call <expression>,<parm1>,<parm2>,<parm3>...)
        当 make 执行这个函数时， <expression>参数中的变量，如$(1)， $(2)， $(3)等，会被参数<p
        arm1>， <parm2>， <parm3>依次取代。而<expression>的返回值就是 call 函数的返回值。例
        如：
        reverse = $(1) $(2)
        foo = $(call reverse,a,b)
        那么， foo 的值就是“a b”。当然，参数的次序是可以自定义的，不一定是顺序的，如：
        reverse = $(2) $(1)
        foo = $(call reverse,a,b)
        此时的 foo 的值就是“b a”。
    strip:    
        $(strip <string>)
        名称：去空格函数——strip。
        功能：去掉<string>字串中开头和结尾的空字符。
        返回：返回被去掉空格的字符串值。
        示例：
        $(strip a b c )
        把字串“a b c ”去到开头和结尾的空格，结果是“a b c”
    findstring:    
        $(findstring <find>,<in>)
        名称：查找字符串函数——findstring。
        功能：在字串<in>中查找<find>字串。
        返回：如果找到，那么返回<find>，否则返回空字符串。
        示例：
        $(findstring a,a b c)
        $(findstring a,b c)
        第一个函数返回“a”字符串，第二个返回“”字符串（空字符串）
    filter:
        $(filter <pattern...>,<text>)
        名称：过滤函数——filter。
        功能：以<pattern>模式过滤<text>字符串中的单词，保留符合模式<pattern>的单词。
        可以有多个模式。
        返回：返回符合模式<pattern>的字串。
        示例：
        sources := foo.c bar.c baz.s ugh.h
        foo: $(sources)
            cc $(filter %.c %.s,$(sources)) -o foo
        $(filter %.c %.s,$(sources))返回的值是“foo.c bar.c baz.s”。
    filter-out:    
        $(filter-out <pattern...>,<text>)
        名称：反过滤函数——filter-out。
        功能：以<pattern>模式过滤<text>字符串中的单词，去除符合模式<pattern>的单词。
        可以有多个模式。
        返回：返回不符合模式<pattern>的字串。
        示例：
        objects=main1.o foo.o main2.o bar.o
        mains=main1.o main2.o
        $(filter-out $(mains),$(objects)) 返回值是“foo.o bar.o”
    sort:
        $(sort <list>)
        名称：排序函数——sort。
        功能：给字符串<list>中的单词排序（升序）。
        返回：返回排序后的字符串。
        示例： $(sort foo bar lose)返回“bar foo lose” 。
        备注： sort 函数会去掉<list>中相同的单词。
    word:
        $(word <n>,<text>)
        名称：取单词函数——word。
        功能：取字符串<text>中第<n>个单词。（从一开始）
        返回：返回字符串<text>中第<n>个单词。如果<n>比<text>中的单词数要大，那么返回
        空字符串。
        示例： $(word 2, foo bar baz)返回值是“bar”。
    wordlist:    
        $(wordlist <s>,<e>,<text>)
        名称：取单词串函数——wordlist。
        功能：从字符串<text>中取从<s>开始到<e>的单词串。 <s>和<e>是一个数字。
        返回：返回字符串<text>中从<s>到<e>的单词字串。如果<s>比<text>中的单词数要大
        ，那么返回空字符串。如果<e>大于<text>的单词数，那么返回从<s>开始，到<text>结束的
        单词串。
        示例： $(wordlist 2, 3, foo bar baz)返回值是“bar baz”。
    words:
        $(words <text>)
        名称：单词个数统计函数——words。
        功能：统计<text>中字符串中的单词个数。
        返回：返回<text>中的单词数。
        示例： $(words, foo bar baz)返回值是“3”。
        备注：如果我们要取<text>中最后的一个单词，我们可以这样： $(word $(words <text>),<text>)。
    firstword:
        $(firstword <text>)
        名称：首单词函数——firstword。
        功能：取字符串<text>中的第一个单词。
        返回：返回字符串<text>的第一个单词。
        示例： $(firstword foo bar)返回值是“foo”。
        备注：这个函数可以用 word 函数来实现： $(word 1,<text>)。
    foreach:
        foreach 函数和别的函数非常的不一样。因为这个函数是用来做循环用的， Makefile 中的
        foreach 函数几乎是仿照于 Unix 标准 Shell（ /bin/sh）中的 for 语句，或是 C-Shell（ /bin/csh）
        中的 foreach 语句而构建的。它的语法是：
        $(foreach <var>,<list>,<text>)
        这个函数的意思是，把参数<list>中的单词逐一取出放到参数<var>所指定的变量中，然后再
        执行<text>所包含的表达式。每一次<text>会返回一个字符串，循环过程中， <text>的所返回
        的每个字符串会以空格分隔，最后当整个循环结束时， <text>所返回的每个字符串所组成的
        整个字符串（以空格分隔）将会是 foreach 函数的返回值。
        所以， <var>最好是一个变量名， <list>可以是一个表达式，而<text>中一般会使用<var>这个
        参数来依次枚举<list>中的单词。举个例子：
        names := a b c d
        files := $(foreach n,$(names),$(n).o)
        上面的例子中， $(name)中的单词会被挨个取出，并存到变量“n”中， “$(n).o”每次根据“$(n)”
        计算出一个值，这些值以空格分隔，最后作为 foreach 函数的返回，所以， $(files)的值是“a.o
        b.o c.o d.o”。
        注意， foreach 中的<var>参数是一个临时的局部变量， foreach 函数执行完后，参数<var>的
        变量将不在作用，其作用域只在 foreach 函数当中。
定义命令包:
    如果 Makefile 中出现一些相同命令序列，那么我们可以为这些相同的命令序列定义一个变量。定义这种命令序列的语法以“define”开始，以“endef”结束
    define my-dir
    $(strip \
    $(eval LOCAL_MODULE_MAKEFILE := $$(lastword $$(MAKEFILE_LIST))) \
    $(eval LOCAL_MODULE_MAKEFILE_DEP := $(if $(BUILDING_WITH_NINJA),,$$(LOCAL_MODULE_MAKEFILE))) \
    $(if $(filter $(BUILD_SYSTEM)/% $(OUT_DIR)/%,$(LOCAL_MODULE_MAKEFILE)), \
        $(error my-dir must be called before including any other makefile.) \
    , \
        $(patsubst %/,%,$(dir $(LOCAL_MODULE_MAKEFILE))) \
    ) \
    )
    endef



分支:
    #若value1与value2值相等，执行ifeq至else部分的代码
    ifeq ($value1, $value2)
    else
    endif
    #若value1与value2值相等，执行else至endif部分的代码
    ifneq ($value1, $value2)
    else
    endif
${xxx}与$(xxx)之间的区别
    ${xxx}:
        1.在shell中,${ }用于变量替换。一般情况下，$var 与${var} 并没有啥不一样。但是用 ${ } 会比较精确的界定变量名称的范围。
        2.在Makefile中,变量引用与$()完全一样

    $()
        1.在shell中，都是进行数学运算的。支持+ - * / %：分别为 “加、减、乘、除、取模”。但是注意，bash只能作整数运算，对于浮点数是当作字符串处理的。
        2.在Makefile中,变量引用与${}完全一样
:=,+=,=,?=之间的区别：
    = 是最基本的赋值
    := 是覆盖之前的值
    ?= 是如果没有被赋值过就赋予等号后面的值
    += 是添加等号后面的值，并且在原有的字符串后加上一个空格后在添加新的值
    其中=与:=号之间的区别:
        对于=
            x = foo
            y = $(x) bar
            x = xyz
        对于:=
            x = foo
            y = $(x) bar
            x = xyz
比较重要的变量
    $@ 扩展成当前规则的目的文件名
    $< 扩展成依靠列表中的第一个依靠文件
    $^ 扩展成整个依靠的列表（除掉了里面所有重 复的文件名）
.PHONY
    .PHONY后面的target表示的也是一个伪造的target, 而不是真实存在的文件target，注意Makefile的target默认是文件。
include:
    在 Makefile 使用 include 关键字可以把别的 Makefile 包含进来，这很像 C 语言的#include，被包含的文件会原模原样的放在当前文件的包含位置。
    filename 可以是当前操作系统 Shell 的文件模式
一些Makefile的高级工具:



