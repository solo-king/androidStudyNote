

#define LOG_TAG "NiaoRenService"

#include"include/BnNiaoRenService.h"
#include<iostream>

namespace android{
    void BnNiaoRenService::loveYou(){

        ALOGI("loveYou:I love ss!!");
    }

    int BnNiaoRenService::myAge(){

        ALOGI("Age:[age]%d", age);
        return age;
    }

    status_t BnNiaoRenService::onTransact( uint32_t code,
                                const Parcel& data,
                                Parcel* reply,
                                uint32_t flags){
        switch (code)
        {
            case INIAOREN_SVR_LOVEYOU:{
                BnNiaoRenService::loveYou();
                reply->writeInt32(0);
                return NO_ERROR;
            }
            case INIAOREN_SVR_MYAGE:{
                int age = BnNiaoRenService::myAge();
                ALOGI("onTransact:[age]%d", age);
                reply->writeInt32(0);
                reply->writeInt32(age);
                return NO_ERROR;
            }
            default:
                return BBinder::onTransact(code, data, reply, flags);
        }
    }
    
}





