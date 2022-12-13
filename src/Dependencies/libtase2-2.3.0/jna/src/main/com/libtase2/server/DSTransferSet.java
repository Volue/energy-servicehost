package com.libtase2.server;

import java.util.HashMap;
import java.util.Map;

import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;

/**
 * This class represents a Data set transfer set (DSTS)
 */
public class DSTransferSet {
    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        Pointer Tase2_DSTransferSet_getDataSetName(Pointer self);

        int Tase2_DSTransferSet_getStartTime(Pointer self);

        int Tase2_DSTransferSet_getInterval(Pointer self);

        int Tase2_DSTransferSet_getTLE(Pointer self);

        int Tase2_DSTransferSet_getBufferTime(Pointer self);

        int Tase2_DSTransferSet_getIntegrityCheck(Pointer self);

        boolean Tase2_DSTransferSet_getBlockData(Pointer self);

        boolean Tase2_DSTransferSet_getCritical(Pointer self);

        boolean Tase2_DSTransferSet_getRBE(Pointer self);

        boolean Tase2_DSTransferSet_getAllChangesReported(Pointer self);

        boolean Tase2_DSTransferSet_getStatus(Pointer self);

        short Tase2_DSTransferSet_getEventCodeRequested(Pointer self);

    }

    private static Map<String, ?> getOptions() {
        Map<String, Object> result = new HashMap<String, Object>();

        DefaultTypeMapper typeMapper = new DefaultTypeMapper();

        typeMapper.addTypeConverter(Boolean.class, new TypeConverter() {
            @Override
            public Object fromNative(Object nativeValue, FromNativeContext context) {
                return ((Byte) nativeValue) != 0;
            }

            @Override
            public Class<?> nativeType() {
                return byte.class;
            }

            @Override
            public Object toNative(Object value, ToNativeContext context) {
                if (value.getClass() == Boolean.class) {
                    value = (Boolean) value;
                }
                if ((Boolean) value) {
                    return (byte) 1;
                } else {
                    return 0;
                }
            }
        });

        result.put(Library.OPTION_TYPE_MAPPER, typeMapper);
        return result;
    }

    Pointer self;

    private String name;

    DSTransferSet(Pointer ptr, String name) {
        self = ptr;
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public String getDataSetName() {
        Pointer dataSetNamePtr = NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getDataSetName(self);

        String retVal = dataSetNamePtr.getString(0);

        Native.free(Pointer.nativeValue(dataSetNamePtr));

        return retVal;
    }

    public int getStartTime() {
        return NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getStartTime(self);
    }

    public int getInterval() {
        return NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getInterval(self);
    }

    public int getTLE() {
        return NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getTLE(self);
    }

    public int getBufferTime() {
        return NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getBufferTime(self);
    }

    public int getIntegrityCheck() {
        return NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getIntegrityCheck(self);
    }

    public boolean getBlockData() {
        return NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getBlockData(self);
    }

    public boolean getCritical() {
        return NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getCritical(self);
    }

    public boolean geRBE() {
        return NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getRBE(self);
    }

    public boolean getAllChangesReported() {
        return NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getAllChangesReported(self);
    }

    public boolean getStatus() {
        return NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getStatus(self);
    }

    public short getEventCodeRequested() {
        return NativeLibTase2.INSTANCE.Tase2_DSTransferSet_getEventCodeRequested(self);
    }
}
