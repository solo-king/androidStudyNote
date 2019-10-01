参考资料:
	//api参考手册
	http://www.cplusplus.com/reference/string/string/?kw=string
c++类的初始化
    string name;    
    string name1(name); //直接初始化
    string name2 = s1;//拷贝初始化
    string name3 = "chenqigan";//拷贝初始化
    string name(10, 'c');//直接初始化
virtual void addService(bool allowIsolated = false) = 0;在使用时是否可以不填写 allowIsolated
    可以
c++对象构造方式
    sp<ProcessState> proc(ProcessState::self());
字符串的概念及使用
    string16("hello")//使用直接初始化方式建立一个hello字符串
关键字virtual的使用
    虚函数
        在基类中定义后，可以实现多态
    虚类与存在虚函数的区别
        纯虚函数用于指定规范，且含有纯虚函数的基类不应该被实例化
c++中的权限关键字
    public:
        公开
    protected:
        1.派生类可以直接访问
        2.只能修改派生类中的age，例子见
            /home/chenqigan/work/test/cppc/cpp/privateTest/privateTest.cpp
            setSelfAge()
    private:
        只能在本类中被访问
继承一个类时，再起前方添加权限控制含义，例如class BpHelloService: public BpInterface<IHelloService>
    1.此处意味着BpHelloService以public形式继承BpInterface。即意味着BpInterface中的public成员也可以通过BpHelloService访问
    2.如果此处BpHelloService以private继承BpInterface<IHelloService>,那么BpInterface<IHelloService>中的protected、public只能在BpHelloService中访问
友元:
    1.友元类
        class A {
            friend class B;
        };
    2.友元函数
        class A {
            friend class B;
        };
template<typename INTERFACE>模板关键字的使用
    Note:其中typename可以使用class代替，并且class的用法要早于typename故有的代码会出现此种写法
    泛型函数:
        template <typename T>  T getT(T t){  return t;}
        template <typename T,B,C,A>  T getT(T t){  return t;}
    泛型类
        template <typename T> class Person{

                private:
                    T age;
                public:
                    T getAge(){};
        };
inline void decStrong() const{}
    此处const的含义:
联合体:
using使用方法:
    可以重新定义一个命名空间的名字，如果该命名空间名字过长,例子如下
        1.namespace ganye{...};
            可以使用namespace gan = ganye;替换
        2.using 定义别名，参考资料：https://en.cppreference.com/w/cpp/language/type_alias
            using getSupportedTypes_cb = std::function<void(const ::android::hardware::hidl_vec<Type>& types)>;
            方法验证见:
                https://github.com/solo-king/cppTestAbout/commit/8e1436d14318799bceb0514305f65a6f501f731d
    
vector:
    1.vector 能容纳绝大部分的对象
    2.应用不是对象故vector不能包含应用数据类型
    3.定义和初始化
        c++11中支持初始化列表:
            vector<string> strings = {"123", "23", "34"};
    4.在遍历 vertor strings 的过程中，不应该向strings中添加新元素
    5.可以用下标访问内部的元素，但是不能添加
引用&:
        
