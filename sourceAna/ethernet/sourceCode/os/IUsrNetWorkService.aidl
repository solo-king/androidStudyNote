package android.os;

/** {@hide} */
interface IUsrNetWorkService
{
    /*UsrMobileManager.java提供接口
    *对移动网络网卡的一些操作
    */
    boolean setMoblieDataStatus(boolean enabled);
    long getMobileTxBytes();
    long getMobileRxBytes();
    long getMobileRxAndTxBytes();
    void getStm8Table();
    String getValueTest(String mobileCardName, String propertyName);
    boolean setValueTest(String mobileCardName, String propertyName, String propertyValue);
    void checkMobileCardsItemTest();
   // UsrMobileManager getmUsrMobileManager();
    void setEthEnabled(boolean enable);
}
