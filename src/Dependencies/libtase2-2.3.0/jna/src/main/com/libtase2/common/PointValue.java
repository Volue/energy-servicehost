package com.libtase2.common;

import java.util.HashMap;
import java.util.Map;

import com.libtase2.client.ClientError;
import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;

/*
 *  Copyright 2018-2019 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;
import com.sun.jna.ptr.PointerByReference;

public class PointValue {
    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        int Tase2_PointValue_getError(Pointer self);

        int Tase2_PointValue_getValueDiscrete(Pointer self);

        void Tase2_PointValue_setValueDiscrete(Pointer self, int value);

        float Tase2_PointValue_getValueReal(Pointer self);

        void Tase2_PointValue_setValueReal(Pointer self, float value);

        byte Tase2_PointValue_getValueState(Pointer self);

        void Tase2_PointValue_setValueState(Pointer self, byte value);

        int Tase2_PointValue_getType(Pointer self);

        void Tase2_PointValue_setType(Pointer self, int type);

        Pointer Tase2_PointValue_getCopy(Pointer self);

        void Tase2_PointValue_destroy(Pointer self);

        Pointer Tase2_PointValue_createReal(float value);

        Pointer Tase2_PointValue_createDiscrete(int value);

        Pointer Tase2_PointValue_createState(byte value);

        Pointer Tase2_PointValue_createStateSupplemental(byte value);

        Pointer Tase2_PointValue_createRealQ(float value, byte flags);

        Pointer Tase2_PointValue_createDiscreteQ(int value, byte flags);

        Pointer Tase2_PointValue_createStateQ(byte value, byte flags);

        Pointer Tase2_PointValue_createStateSupplementalQ(byte value, byte flags);

        Pointer Tase2_PointValue_createRealQTimeTag(float value, byte flags, int timeTag);

        Pointer Tase2_PointValue_createStateQTimeTag(byte valueAndFlags, int timeTag);

        Pointer Tase2_PointValue_createDiscreteQTimeTag(int value, byte flags, int timeTag);

        Pointer Tase2_PointValue_createStateSupplementalQTimeTag(byte value, byte flags, int timeTag);

        Pointer Tase2_PointValue_createRealExtended(float value, byte flags, int timeTag, short cov);

        Pointer Tase2_PointValue_createStateExtended(byte valueAndFlags, int timeTag, short cov);

        Pointer Tase2_PointValue_createDiscreteExtended(int value, byte flags, int timeTag, short cov);

        Pointer Tase2_PointValue_createStateSupplementalExtended(byte value, byte flags, int timeTag, short cov);

        Pointer Tase2_PointValue_createRealQTimeTagExtended(float value, byte flags, long timeTagExtended);

        Pointer Tase2_PointValue_createStateQTimeTagExtended(byte valueAndFlags, long timeTagExtended);

        Pointer Tase2_PointValue_createDiscreteQTimeTagExtended(int value, byte flags, long timeTagExtended);

        Pointer Tase2_PointValue_createStateSupplementalQTimeTagExtended(byte value, byte flags, long timeTagExtended);

        long Tase2_PointValue_getTimeStampEx(Pointer self, PointerByReference err);

        byte Tase2_PointValue_getFlagsEx(Pointer self, PointerByReference err);

        short Tase2_PointValue_getCOVEx(Pointer self, PointerByReference err);

        int Tase2_PointValue_getTimeStampClass(Pointer self);
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

    private Pointer self;

    public PointValue(Pointer self) {
        this.self = self;
    }

    public PointValue(Pointer ptr, boolean copy) {
        if (copy)
            self = NativeLibTase2.INSTANCE.Tase2_PointValue_getCopy(ptr);
        else
            self = ptr;
    }

    public static PointValue createReal(float value) {
        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_PointValue_createReal(value);
        return new PointValue(ptr);
    }

    public static PointValue createDiscrete(int value) {
        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_PointValue_createDiscrete(value);
        return new PointValue(ptr);
    }

    /**
     * 
     * @param value state value. One or more of the values of DataState
     * @return
     */
    public static PointValue createState(int value) {
        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_PointValue_createState((byte) value);
        return new PointValue(ptr);
    }

    /**
     * 
     * @param value state value. One or more of the values of DataStateSupplemental
     * @return
     */
    public static PointValue createStateSupplemental(int value) {
        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_PointValue_createStateSupplemental((byte) value);
        return new PointValue(ptr);
    }

    /**
     * Create new PointValue instance of type RealQ
     * 
     * @param value Real value.
     * @param flags the quality flags (values of DataFlags
     * @return
     */
    public static PointValue createRealQ(float value, int flags) {
        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_PointValue_createRealQ(value, (byte) flags);
        return new PointValue(ptr);
    }

    /**
     * Get the (estimated) type of the point value
     *
     * @return type information
     */
    public PointValueType getType() {
        int typeVal = NativeLibTase2.INSTANCE.Tase2_PointValue_getType(self);

        return PointValueType.fromValue(typeVal);
    }

    /**
     * Get the error code (when point value type is PointValueType.ERROR)
     *
     * This function is intended to get individual error codes for each data point
     * when reading multiple data points at once.
     *
     * @return the error code
     */
    public ClientError getError() {
        int errorVal = NativeLibTase2.INSTANCE.Tase2_PointValue_getError(self);

        return ClientError.fromValue(errorVal);
    }

    /**
     * Set the type of a point value.
     *
     * The elements of a point value can only be interpreted correctly when the
     * TASE.2 type is known. In most cases the TASE.2 type can be derived from the
     * MMS type of the MMS value that has been received. However, in some cases this
     * is not possible because the same MMS type is used for different TASE.2 types.
     * In this case it is required to tell the library what TASE.2 type has to be
     * assumed to interpret the value elements. When you get a type UNKNOWN,
     * STATE_OR_STATE_SUPPLEMENTAL, or STATE_OR_DISCRETE you have to use this
     * function to advise the correct type.
     *
     * @param type the TASE.2 type to be used for value interpretation.
     */
    public void setType(PointValueType type) {
        NativeLibTase2.INSTANCE.Tase2_PointValue_setType(self, type.getValue());
    }

    public int getValueDiscrete() {
        return NativeLibTase2.INSTANCE.Tase2_PointValue_getValueDiscrete(self);
    }

    public void setValueDiscrete(int value) {
        NativeLibTase2.INSTANCE.Tase2_PointValue_setValueDiscrete(self, value);
    }

    public float getValueReal() {
        return NativeLibTase2.INSTANCE.Tase2_PointValue_getValueReal(self);
    }

    public void setValueRead(float value) {
        NativeLibTase2.INSTANCE.Tase2_PointValue_setValueReal(self, value);
    }

    public int getValueState() {
        byte stateVal = NativeLibTase2.INSTANCE.Tase2_PointValue_getValueState(self);

        return (stateVal & 0xff);
    }

    public void setValueState(int value) {
        NativeLibTase2.INSTANCE.Tase2_PointValue_setValueState(self, (byte) value);
    }

    /**
     * Get the time stamp of the last data point change or update
     * 
     * @return the time stamp in milliseconds since epoch
     * @throws PointValueException in case the PointValue instance has no timestamp
     */
    public long getTimeStamp() throws PointValueException {
        PointerByReference err = new PointerByReference();

        long timestampValue = NativeLibTase2.INSTANCE.Tase2_PointValue_getTimeStampEx(self, err);

        int errorCode = err.getPointer().getInt(0);

        if (errorCode != 0)
            throw new PointValueException("value has not timestamp", PointValueError.fromValue(errorCode));

        return timestampValue;
    }

    /**
     * Get the timestamp class of the value
     * 
     * NOTE: When the getType function returns STATE_OR_DISCRETE the function cannot
     * determine if there is a timestamp or not. In this case the function returns
     * TimeStampClass.NO_TIMESTAMP
     * 
     * @return
     */
    public TimeStampClass getTimeStampClass() {
        int timestampClassInt = NativeLibTase2.INSTANCE.Tase2_PointValue_getTimeStampClass(self);

        return TimeStampClass.fromValue(timestampClassInt);
    }

    /**
     * Get the quality flags
     * 
     * @return the quality flags
     * @throws PointValueException in case the PointValue instance has no quality
     *                             flags
     */
    public DataFlags getFlags() throws PointValueException {
        PointerByReference err = new PointerByReference();

        byte flagsValue = NativeLibTase2.INSTANCE.Tase2_PointValue_getFlagsEx(self, err);

        int errorCode = err.getPointer().getInt(0);

        if (errorCode != 0)
            throw new PointValueException("value has no flags", PointValueError.fromValue(errorCode));

        return new DataFlags(flagsValue);
    }

    /**
     * Get the COV (counter of value changes) of a point value
     * 
     * NOTE: The COV value is in the range 0..65535
     * 
     * @return the COV value
     * @throws PointValueException in case the PointVAlue instance has no COV value
     */
    public int getCOV() throws PointValueException {
        PointerByReference err = new PointerByReference();

        short covValue = NativeLibTase2.INSTANCE.Tase2_PointValue_getCOVEx(self, err);

        int errorCode = err.getPointer().getInt(0);

        if (errorCode != 0)
            throw new PointValueException("value has no COV", PointValueError.fromValue(errorCode));

        return (covValue & 0xffff);
    }

    public void dispose() {
        if (self != null) {
            NativeLibTase2.INSTANCE.Tase2_PointValue_destroy(self);
            self = null;
        }
    }

    @Override
    public String toString() {
        String retStr = "";

        switch (getType()) {
        case DISCRETE:
            retStr = "" + getValueDiscrete();
            break;

        case REAL:
            retStr = "" + getValueReal();
            break;

        case STATE:
        case STATE_SUPPLEMENTAL:
        case STATE_OR_STATE_SUPPLEMENTAL:
            retStr = "" + getValueState();
            break;

        case ERROR:
            retStr = getError().toString();
            break;

        default:
            break;
        }

        return retStr;
    }

    public void finalize() {
        dispose();
    }

}
