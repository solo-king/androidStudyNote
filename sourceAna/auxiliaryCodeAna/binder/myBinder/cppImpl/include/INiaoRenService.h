#ifndef ANDROID_INIAORENSERVICE_H
#define ANDROID_INIAORENSERVICE_H


#include<binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/Errors.h>  // for status_t
#include <utils/RefBase.h>



#define INIAOREN_SVR_LOVEYOU              1
#define INIAOREN_SVR_MYAGE                2

namespace android{

    /* 接口类定义 */
    class INiaoRenService :public IInterface
    {

        public:
           virtual void loveYou()=0;
           virtual int myAge()=0;

           DECLARE_META_INTERFACE(NiaoRenService);
    };

}
#endif
