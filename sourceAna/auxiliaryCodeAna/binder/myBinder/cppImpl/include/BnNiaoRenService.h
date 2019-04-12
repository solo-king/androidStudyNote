#ifndef ANDROID_NIAORENSERVICE_H
#define ANDROID_NIAORENSERVICE_H

#include "INiaoRenService.h"

namespace android{
    
    /* 功能的实现类 */
    class BnNiaoRenService:public BnInterface<INiaoRenService>{

        public:
            BnNiaoRenService(int age){

                this->age = age;
            }
            virtual void loveYou();
            virtual int myAge();
            virtual status_t    onTransact( uint32_t code,
                                        const Parcel& data,
                                        Parcel* reply,
                                        uint32_t flags = 0);
        private:
            int age;
    };
}

#endif