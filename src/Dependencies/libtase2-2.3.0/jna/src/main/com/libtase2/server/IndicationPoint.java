package com.libtase2.server;

import java.util.HashMap;
import java.util.Map;

/*
 *  Copyright 2018-2019 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import com.libtase2.common.DataFlags;
import com.libtase2.common.DataState;
import com.libtase2.common.DataStateSupplemental;
import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;

public class IndicationPoint extends DataPoint {

    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        void Tase2_IndicationPoint_setReal(Pointer self, float value);

        void Tase2_IndicationPoint_setRealQ(Pointer self, float value, byte flags);

        void Tase2_IndicationPoint_setRealQTimeStamp(Pointer self, float value, byte flags, long timestamp);

        void Tase2_IndicationPoint_setDiscrete(Pointer self, int value);

        void Tase2_IndicationPoint_setDiscreteQ(Pointer self, int value, byte flags);

        void Tase2_IndicationPoint_setDiscreteQTimeStamp(Pointer self, int value, byte flags, long timestamp);

        void Tase2_IndicationPoint_setState(Pointer self, byte value);

        void Tase2_IndicationPoint_setStateTimeStamp(Pointer self, byte value, long timestamp);

        void Tase2_IndicationPoint_setStateSupplemental(Pointer self, byte value);

        void Tase2_IndicationPoint_setStateSupplementalQ(Pointer self, byte value, byte flags);

        void Tase2_IndicationPoint_setStateSupplementalQTimeStamp(Pointer self, byte value, byte flags, long timestamp);

        void Tase2_IndicationPoint_setQuality(Pointer self, byte flags);

        void Tase2_IndicationPoint_setTimeStamp(Pointer self, long timestamp);

        void Tase2_IndicationPoint_setCOV(Pointer self, short cov);

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

    protected IndicationPoint(Pointer ptr, String name, Domain domain) {
        super(ptr, name, domain);
    }

    /**
     * Sets the quality flags of a data point
     * 
     * @param flags the quality flags values
     */
    public void setQuality(DataFlags flags) {
        byte nativeFlags = (byte) flags.getValue();

        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setQuality(self, nativeFlags);
    }

    /**
     * Sets the timestamp of a data point
     * 
     * NOTE: This has no effect when the data point type has no timestamp.
     * 
     * @param timestamp
     */
    public void setTimeStamp(long timestamp) {
        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setTimeStamp(self, timestamp);
    }

    /**
     * Set the COV (cointer of value changes) of the data point
     * 
     * NOTE: This has no effect when the data point type has no COV.
     * 
     * @param cov the COV value (allowed range is 0 to 65535)
     */
    public void setCOV(int cov) {
        if ((cov < 0) || (cov > 65535))
            throw new IllegalArgumentException("For COV only the range 0 to 65535 is allowed.");

        short nativeCov = (short) cov;

        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setCOV(self, nativeCov);
    }

    /**
     * Set the value of a data point of type
     * {@link com.libtase2.server.IndicationPointType#REAL REAL}
     * 
     * @param value new value
     */
    public void setReal(float value) {
        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setReal(self, value);
    }

    /**
     * Set the value of a data point of type
     * {@link com.libtase2.server.IndicationPointType#REAL REAL} with quality flags
     * 
     * @param value new value
     * @param flags the quality flags
     */
    public void setRealQ(float value, DataFlags flags) {
        byte nativeFlags = (byte) flags.getValue();

        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setRealQ(self, value, nativeFlags);
    }

    /**
     * Set the value of a data point of type
     * {@link com.libtase2.server.IndicationPointType#REAL REAL} with quality flags
     * and timestamp
     * 
     * @param value     new value
     * @param flags     the quality flags
     * @param timestamp (ms since epoch)
     */
    public void setRealQTimeStamp(float value, DataFlags flags, long timestamp) {
        byte nativeFlags = (byte) flags.getValue();

        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setRealQTimeStamp(self, value, nativeFlags, timestamp);
    }

    /**
     * Set the value of a data point of type
     * {@link com.libtase2.server.IndicationPointType#DISCRETE DISCRETE}
     * 
     * @param value new value
     */
    public void setDiscrete(int value) {
        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setDiscrete(self, value);
    }

    /**
     * Set the value of a data point of type
     * {@link com.libtase2.server.IndicationPointType#DISCRETE DISCRETE} with
     * quality flags
     * 
     * @param value new value
     * @param flags the quality flags
     */
    public void setDiscreteQ(int value, DataFlags flags) {
        byte nativeFlags = (byte) flags.getValue();

        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setDiscreteQ(self, value, nativeFlags);
    }

    /**
     * Set the value of a data point of type
     * {@link com.libtase2.server.IndicationPointType#DISCRETE DISCRETE} with
     * quality flags and timestamp
     * 
     * @param value     new value
     * @param flags     the quality flags
     * @param timestamp (ms since epoch)
     */
    public void setDiscreteQTimeStamp(int value, DataFlags flags, long timestamp) {
        byte nativeFlags = (byte) flags.getValue();

        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setDiscreteQTimeStamp(self, value, nativeFlags, timestamp);
    }

    /**
     * Set the value of a data point of type
     * {@link com.libtase2.server.IndicationPointType#STATE STATE}
     * 
     * @param value new value (and quality flags)
     */
    public void setState(DataState state) {
        byte nativeVal = (byte) state.getValue();

        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setState(self, nativeVal);
    }

    /**
     * Set the value of a data point of type
     * {@link com.libtase2.server.IndicationPointType#STATE STATE} with quality
     * flags and timestamp
     * 
     * @param value     new value and quality flags
     * @param timestamp (ms since epoch)
     */
    public void setStateTimeStamp(DataState state, long timestamp) {
        byte nativeVal = (byte) state.getValue();

        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setStateTimeStamp(self, nativeVal, timestamp);
    }

    /**
     * Set the value of a data point of type
     * {@link com.libtase2.server.IndicationPointType#STATE_SUPPLEMENTAL
     * STATE_SUPPLEMENTAL}
     * 
     * @param value new value
     */
    public void setStateSupplemental(DataStateSupplemental state) {
        byte nativeState = (byte) state.getValue();

        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setStateSupplemental(self, nativeState);
    }

    /**
     * Set the value of a data point of type
     * {@link com.libtase2.server.IndicationPointType#STATE_SUPPLEMENTAL
     * STATE_SUPPLEMENTAL} with quality flags
     * 
     * @param value new value
     * @param flags the quality flags
     */
    public void setStateSupplementalQ(DataStateSupplemental state, DataFlags flags) {
        byte nativeState = (byte) state.getValue();
        byte nativeFlags = (byte) flags.getValue();

        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setStateSupplementalQ(self, nativeState, nativeFlags);
    }

    /**
     * Set the value of a data point of type
     * {@link com.libtase2.server.IndicationPointType#STATE_SUPPLEMENTAL
     * STATE_SUPPLEMENTAL} with quality flags and timestamp
     * 
     * @param value     new value
     * @param flags     the quality flags
     * @param timestamp (ms since epoch)
     */
    public void setStateSupplementalQTimeStamp(DataStateSupplemental state, DataFlags flags, long timestamp) {
        byte nativeState = (byte) state.getValue();
        byte nativeFlags = (byte) flags.getValue();

        NativeLibTase2.INSTANCE.Tase2_IndicationPoint_setStateSupplementalQTimeStamp(self, nativeState, nativeFlags,
                timestamp);
    }
}
