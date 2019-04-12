1.c++中创建一个java对象
2.c++中设置java对象中的成员变量
3.c++中调用java对象中的方法
jobject res = jniGetReferent(env, object);
env->DeleteGlobalRef(object);
object = env->NewObject(gBinderProxyOffsets.mClass, gBinderProxyOffsets.mConstructor);
jobject refObject = env->NewGlobalRef(
env->GetObjectField(object, gBinderProxyOffsets.mSelf));
env->SetLongField(object, gBinderProxyOffsets.mOrgue, reinterpret_cast<jlong>(drl.get()));