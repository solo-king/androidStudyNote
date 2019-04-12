/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: frameworks/base/core/java/android/os/IUsrLedService.aidl
 */
package android.os;

public interface IUsrLedService extends android.os.IInterface {
    /** Local-side IPC implementation stub class. */
    public static abstract class Stub extends android.os.Binder implements android.os.IUsrLedService {
        private static final java.lang.String DESCRIPTOR = "android.os.IUsrLedService";

        /** Construct the stub at attach it to the interface. */
        public Stub() {
            this.attachInterface(this, DESCRIPTOR);
        }

        /**
         * Cast an IBinder object into an android.os.IUsrLedService interface,
         * generating a proxy if needed.
         */
        public static android.os.IUsrLedService asInterface(android.os.IBinder obj) {
            if ((obj == null)) {
                return null;
            }
            android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
            if (((iin != null) && (iin instanceof android.os.IUsrLedService))) {
                return ((android.os.IUsrLedService) iin);
            }
            return new android.os.IUsrLedService.Stub.Proxy(obj);
        }

        @Override
        public android.os.IBinder asBinder() {
            return this;
        }

        @Override
        public boolean onTransact(int code, android.os.Parcel data, android.os.Parcel reply, int flags)
                throws android.os.RemoteException {
            switch (code) {
            case INTERFACE_TRANSACTION: {
                reply.writeString(DESCRIPTOR);
                return true;
            }
            case TRANSACTION_openLedDevices: {
                data.enforceInterface(DESCRIPTOR);
                int _result = this.openLedDevices();
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_ledCtrl: {
                data.enforceInterface(DESCRIPTOR);
                int _arg0;
                _arg0 = data.readInt();
                int _result = this.ledCtrl(_arg0);
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_signalCtrl: {
                data.enforceInterface(DESCRIPTOR);
                int _arg0;
                _arg0 = data.readInt();
                this.signalCtrl(_arg0);
                reply.writeNoException();
                return true;
            }
            }
            return super.onTransact(code, data, reply, flags);
        }

        private static class Proxy implements android.os.IUsrLedService {
            private android.os.IBinder mRemote;

            Proxy(android.os.IBinder remote) {
                mRemote = remote;
            }

            @Override
            public android.os.IBinder asBinder() {
                return mRemote;
            }

            public java.lang.String getInterfaceDescriptor() {
                return DESCRIPTOR;
            }

            @Override
            public int openLedDevices() throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    mRemote.transact(Stub.TRANSACTION_openLedDevices, _data, _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public int ledCtrl(int cmd) throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(cmd);
                    mRemote.transact(Stub.TRANSACTION_ledCtrl, _data, _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public void signalCtrl(int level) throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(level);
                    mRemote.transact(Stub.TRANSACTION_signalCtrl, _data, _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }
        }

        static final int TRANSACTION_openLedDevices = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
        static final int TRANSACTION_ledCtrl = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
        static final int TRANSACTION_signalCtrl = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
    }

    public int openLedDevices() throws android.os.RemoteException;

    public int ledCtrl(int cmd) throws android.os.RemoteException;

    public void signalCtrl(int level) throws android.os.RemoteException;
}
