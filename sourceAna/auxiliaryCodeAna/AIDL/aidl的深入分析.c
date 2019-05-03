参考资料:
    //官方资料
    https://developer.android.com/guide/components/aidl
    //对ByteBuffer 类的详细说明
    https://developer.android.com/reference/java/nio/ByteBuffer
Android Interface Definition Language (AIDL)
接口定义:
    1.The parameters and return values can be of any type, even other AIDL-generated interfaces.
支持的数据类型:
    1.All primitive types in the Java programming language (such as int, long, char, boolean, and so on)
    2.String、CharSequence
    3.List<type>
        3.1 其中的type必须是被aidl支持的数据类型
        3.2 other AIDL-generated interfaces or parcelables you have declared.
        3.3 对于 List<String>在aidl中定义的，真正被使用时其实是ArrayList类
    4.Map<type>
        4.1 其中的type必须是被aidl支持的数据类型
        4.2 Map<String,Integer>不被支持
        4.3 The actual concrete class that the other side receives is always a HashMap, 
            although the method is generated to use the Map interface.
使用注意事项:
    1. 如果要使用除了在<支持的数据类型>中提到的数据类型需要使用impor 导入，即使是在同一个包下
    2. 在定义服务接口时
        2.1 All non-primitive parameters require a directional tag indicating which way the data goes. 
            Either in, out, or inout (see the example below). 
        2.2 Primitives are in by default, and cannot be otherwise.
        2.3 对于in out  inout的选用一定要在必须的情况下使用，因为定义out inout的使用代价是俺家的
        2.2 All code comments included in the .aidl file are included in the generated IBinder interface (except for comments before the import and package statements).
        2.3 String and int constants can be defined in the AIDL interface. For example: const int VERSION = 1;
        2.4 Annotate nullable arguments or return types using @nullable
    3.服务抛出的异常不会返回给调用者(binder client)
暴露service binder实现的接口
    1.绑定service
传递对象:
    1. you must ensure that the code for your class is available to the other side of the IPC channe
    2. your class must support the Parcelable interface.
        2.1 对于class支持Parcelable接口是必要的，只有支持Parcelable接口，Android系统才能对该class重新组包传送到另一个进程
    3.创建支持parcelable协议的对象
        3.1 Make your class implement the Parcelable interface.
        3.2 Implement writeToParcel, which takes the current state of the object and writes it to a Parcel.
        3.3 Add a static field called CREATOR to your class which is an object implementing the Parcelable.Creator interface.
        3.4 Finally, create an .aidl file that declares your parcelable class (as shown for the Rect.aidl file, below).
    4.在AndroidStudio上创建支持parcelable的对需注意的事项
        4.1 aidl与java必须分开存放到aidl及java目录下，例如
            aidlClient/app/src/main/aidl/cn/usr/www/aildservice/MyPerson.aidl
            aidlClient/app/src/main/java/cn/usr/www/aildservice/MyPerson.java
        4.2 可参考项目见</home/chenqigan/AndroidStudioProjects/aidlClient>
                        </home/chenqigan/AndroidStudioProjects/aildService/>
通过aidl调用另一个进程的方法(Calling an IPC method)
    1.注册一个接口到binder service中去
        private IRemoteServiceCallback mCallback = new IRemoteServiceCallback.Stub() {
            /**
             * This is called by the remote service regularly to tell us about
             * new values.  Note that IPC calls are dispatched through a thread
             * pool running in each process, so the code executing here will
             * NOT be running in our main thread like most other things -- so,
             * to update the UI, we need to use a Handler to hop over there.
             */
            public void valueChanged(int value) {
                handler.sendMessage(handler.obtainMessage(BUMP_MSG, value, 0));
            }
        };
    2.
参数的传输方向:
    in - object is transferred from client to service only used for inputs
        1.仅仅用于service的输入值
    out - object is transferred from client to service only used for outputs.
        1.不传递client的值
        2.但service对client传递过来的参数修改对client可见
    inout - object is transferred from client to service used for both inputs and outputs.
        1.传递client的值
        2.service对client传递过来的参数修改对client可见
需解决的问题:
    传输基本数据类型
    传输自定义对象
    传输文件句柄
    传输ByteBuffer对象
    传输接口
SerialService的使用:
    
