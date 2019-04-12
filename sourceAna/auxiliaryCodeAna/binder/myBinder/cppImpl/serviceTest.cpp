
#include"include/INiaoRenService.h"
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include"include/BnNiaoRenService.h"


using namespace android;


int main(){

    sp<ProcessState> proc(ProcessState::self());
    sp<IServiceManager> sm = defaultServiceManager();
    sm->addService( String16("NiaoRenService"),  new BnNiaoRenService(24));
    
	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();

    return 0;
}