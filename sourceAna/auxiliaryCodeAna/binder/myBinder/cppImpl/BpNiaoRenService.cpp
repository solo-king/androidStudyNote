#include"include/BpNiaoRenService.h"


namespace android{
    void BpNiaoRenService::loveYou(){
        Parcel data, replay;
        data.writeInt32(0);
        data.writeString16(String16("INiaoRenService"));

        remote()->transact(INIAOREN_SVR_LOVEYOU, data, &replay);
    }


    int  BpNiaoRenService::myAge(){

        Parcel data,reply;

        data.writeInt32(0);
        data.writeString16(String16("INiaoRenService"));

        remote()->transact(INIAOREN_SVR_MYAGE, data, &reply);
        reply.readInt32();
        int ret = reply.readInt32();
        return ret;
    }
    /* 与DECLARE_META_INTERFACE()对应，一个申明一个实现
        主要做了如下工作:
            1.初始化descriptor变量
            2.定义getInterfaceDescriptor函数
            3.定义asInterface()函数,主要差异体现在intr = new BpHelloService(obj);阶段
            4.
        */
    
}

