
package android.os;

/** {@hide} */
interface IEPC300CmdCompatibilityService
{
    String infoCmdSend(String cmdFromApp);
    int noInfoCmdSend(String cmdFromApp);
}

