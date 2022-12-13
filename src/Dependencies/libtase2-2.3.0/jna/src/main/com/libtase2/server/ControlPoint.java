package com.libtase2.server;

import java.util.HashMap;
import java.util.Map;

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

/**
 * Represents a control point (device)
 */
public class ControlPoint extends DataPoint {

    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        int Tase2_ControlPoint_getType(Pointer self);
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

    protected ControlPoint(Pointer ptr, String name, Domain domain) {
        super(ptr, name, domain);
    }

    /**
     * Get the type of the control point
     * 
     * @return the control point type
     */
    public ControlPointType getType() {
        int typeVal = NativeLibTase2.INSTANCE.Tase2_ControlPoint_getType(self);

        return ControlPointType.fromValue(typeVal);
    }

}
