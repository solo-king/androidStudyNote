官方参考资料:
    https://developer.android.com/training/basics/network-ops/


ConnectivityManager: 
    Answers queries about the state of network connectivity. It also notifies applications when network connectivity changes.
NetworkInfo: 
    Describes the status of a network interface of a given type (currently either Mobile or Wi-Fi).
对于判断一个网络是否可用应该使用NetworkInfo中的isConnected()而不是available();
对于ConnectionService的实现则是位于Telephony ConnectionService
    The most common ConnectionService implementation on a phone is the Telephony ConnectionService which is responsible for connecting carrier calls
Service State Tracker (SST)
SIM IO subsystem
GsmCdmaCallTracker
Data Connection Tracker (DCT)
SIM toolkit
Support for MMI codes
It also provides us certain services like Voice services ( MO, MT , Call forwarding, call hold, 3-way call, Call supplementary ),SMS , MMS, voice mail,Network Acess Services (NAS-Voice, data and bearer registration ),SIM Services ,Device Management Services(DMS),Phone book Manager(PBM — Managing Contacts).