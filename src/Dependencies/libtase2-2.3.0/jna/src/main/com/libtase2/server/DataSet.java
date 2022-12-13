package com.libtase2.server;

import java.util.HashMap;
import java.util.Map;

import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;

/**
 * Represents a set of TASE.2 variables (indication points, devices, protection
 * equipment)
 */
public class DataSet {
    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        Pointer Tase2_DataSet_addEntry(Pointer self, Pointer domain, String variableName);
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
    private String name;

    DataSet(Pointer ptr, String name) {
        self = ptr;
        this.name = name;
    }

    /**
     * Get the name of the data set
     * 
     * @return the name of the data set
     */
    public String getName() {
        return name;
    }

    /**
     * Add a new entry (variable) to the data sets
     * 
     * @param domain       the domain of the variable or null for VCC scope
     *                     variables
     * @param variableName
     */
    public void addEntry(Domain domain, String variableName) {
        Pointer domainPtr = Pointer.NULL;

        if (domain != null)
            domainPtr = domain.self;

        NativeLibTase2.INSTANCE.Tase2_DataSet_addEntry(self, domainPtr, variableName);
    }

}
