#ifndef ANDROID_BPNIAORENSERVICE_H
#define ANDROID_BPNIAORENSERVICE_H

#include "INiaoRenService.h"
namespace android{

    class BpNiaoRenService:public BpInterface<INiaoRenService>{

        public:
            /* 使用传进来的Ibinder类型构建代理类 */
            BpNiaoRenService(const sp<IBinder>&impl): BpInterface<INiaoRenService>(impl)
            {

            }
            virtual void loveYou();
            virtual int myAge();
    };
    
    IMPLEMENT_META_INTERFACE(NiaoRenService, "android.testing.INiaoRenService");
}


#endif