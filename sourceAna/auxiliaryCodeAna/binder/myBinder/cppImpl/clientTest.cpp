
#define LOG_TAG     "clientTest"

#include"include/INiaoRenService.h"
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include"include/BnNiaoRenService.h"


using namespace android;


static void  usage(char**argv){

    ALOGE("%s <1/2>", argv[0]);
}

int main(int argc, char** argv){


    if(argc != 2){

        usage(argv);
        return -1;
    }

    sp<ProcessState> proc(ProcessState::self());
    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder>  ibinder = sm->getService(String16("NiaoRenService"));

    if(ibinder == 0){
        ALOGE("get service error!!");
        return -1;
    }

    sp<INiaoRenService> service =  interface_cast<INiaoRenService>(ibinder);
    if(!strcmp("1", argv[1])){
        service->loveYou();
    }else if(!strcmp("2", argv[1])){
        int age = service->myAge();
        ALOGD("[age]%d", age);
    }else{
        ALOGD("NOT SURPOOT");
    }
    

    return 0;
}