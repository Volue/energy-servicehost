package com.libtase2.server;

import java.util.HashMap;
import java.util.Map;

import com.libtase2.common.PackedEvent;
import com.libtase2.common.PackedEventFlags;
import com.libtase2.common.SingleEventFlags;
import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import com.sun.jna.Pointer;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;

/**
 * Represents protection equipment data points (protection events)
 */
public class ProtectionEquipment extends DataPoint {
    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        void Tase2_ProtectionEquipment_setSingleEventFlags(Pointer self, byte eventFlags);

        void Tase2_ProtectionEquipment_setPackedEvent(Pointer self, byte packedEvent);

        void Tase2_ProtectionEquipment_setPackedEventFlags(Pointer self, byte flags);

        void Tase2_ProtectionEquipment_setOperatingTime(Pointer self, int operatingTime);

        void Tase2_ProtectionEquipment_setEventTime(Pointer self, long eventTime);

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

    protected ProtectionEquipment(Pointer ptr, String name, Domain domain) {
        super(ptr, name, domain);
    }

    /**
     * Set the event flags of a protection equipment of type SINGLE_EVENT.
     * 
     * @param eventFlags the event flags
     */
    public void setSingleEventFlags(SingleEventFlags eventFlags) {
        byte nativeFlags = (byte) eventFlags.getValue();

        NativeLibTase2.INSTANCE.Tase2_ProtectionEquipment_setSingleEventFlags(self, nativeFlags);
    }

    /**
     * Set the event flags of a protection equipment of type PACKED_EVENT.
     * 
     * @param packedEvent the event flags
     */
    public void setPackedEvent(PackedEvent packedEvent) {
        byte nativeFlags = (byte) packedEvent.getValue();

        NativeLibTase2.INSTANCE.Tase2_ProtectionEquipment_setPackedEvent(self, nativeFlags);
    }

    /**
     * Sets the quality flags of a protection equipment of type PACKED_EVENT.
     * 
     * @param packedEventFlags the quality event flags
     */
    public void setPackedEventFlags(PackedEventFlags packedEventFlags) {
        byte nativeFlags = (byte) packedEventFlags.getValue();

        NativeLibTase2.INSTANCE.Tase2_ProtectionEquipment_setPackedEventFlags(self, nativeFlags);
    }

    /**
     * Sets the duration of the operation/event
     * 
     * @param operatingTime the duration in ms.
     */
    public void setOperatingTime(int operatingTime) {
        NativeLibTase2.INSTANCE.Tase2_ProtectionEquipment_setOperatingTime(self, operatingTime);
    }

    /**
     * Set the event time (or start time of the operation)
     * 
     * @param eventTime the event/start time as ms since epoch.
     */
    public void setEventTime(long eventTime) {
        NativeLibTase2.INSTANCE.Tase2_ProtectionEquipment_setEventTime(self, eventTime);
    }

}
