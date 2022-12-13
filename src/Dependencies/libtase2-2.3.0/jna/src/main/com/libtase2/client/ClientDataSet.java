package com.libtase2.client;

import java.util.HashMap;
import java.util.Map;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import com.libtase2.common.PointValue;
import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;

public class ClientDataSet {

    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        String Tase2_ClientDataSet_getDomainName(Pointer self);

        String Tase2_ClientDataSet_getDataSetName(Pointer self);

        boolean Tase2_ClientDataSet_isDeletable(Pointer self);

        int Tase2_ClientDataSet_getSize(Pointer self);

        String Tase2_ClientDataSet_getPointDomainName(Pointer self, int index);

        String Tase2_ClientDataSet_getPointVariableName(Pointer self, int index);

        Pointer Tase2_ClientDataSet_getPointValue(Pointer self, int index);

        int Tase2_ClientDataSet_read(Pointer self, Pointer client);

        void Tase2_ClientDataSet_destroy(Pointer self);
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

    public Pointer self;

    public ClientDataSet(Pointer self) {
        this.self = self;
    }

    /**
     * Release all resource allocated by the Client instance
     * 
     * <p>
     * NOTE: Do not use the object after calling dispose!
     * </p>
     */
    public void Dispose() {
        if (self != null) {
            NativeLibTase2.INSTANCE.Tase2_ClientDataSet_destroy(self);
            self = null;
        }
    }

    /**
     * Get the domain name of the data set
     *
     * @return the domain name or null, for VCC scope data sets
     */
    public String getDomainName() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDataSet_getDomainName(self);
    }

    /**
     * Get the name of the data set
     *
     * @return the data set name
     */
    public String getDataSetName() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDataSet_getDataSetName(self);
    }

    /**
     * Check if the data set can be deleted by the client
     *
     * @return true when data set is can be deleted by the client, false otherwise
     */
    public boolean isDeletable() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDataSet_isDeletable(self);
    }

    /**
     * Get the size (number of entries) of the data set
     * 
     * @return number of data set entries
     */
    public int getSize() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDataSet_getSize(self);
    }

    /**
     * Get the domain name of the data set entry at the given index
     *
     * @param index the index of the data set entry (starting with 0)
     *
     * @return the domain name of the data set entry
     */
    public String getPointDomainName(int index) {
        return NativeLibTase2.INSTANCE.Tase2_ClientDataSet_getPointDomainName(self, index);
    }

    /**
     * Get the variable name of the data set entry at the given index
     *
     * @param index the index of the data set entry (starting with 0)
     *
     * @return the point name of the data set entry
     */
    public String getPointVariableName(int index) {
        return NativeLibTase2.INSTANCE.Tase2_ClientDataSet_getPointVariableName(self, index);
    }

    /**
     * Get the point value for the data set entry with the given index
     *
     * @param index the index position of the data set entry (starting with 0)
     *
     * @return Return a copy of the data point value.
     */
    public PointValue getPointValue(int index) {
        Pointer pointValuePtr = NativeLibTase2.INSTANCE.Tase2_ClientDataSet_getPointValue(self, index);

        if (pointValuePtr.equals(Pointer.NULL))
            return null;
        else
            return new PointValue(pointValuePtr);
    }

    /**
     * Read the data set values from the server
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the connection to be used
     */
    public void read(Client client) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDataSet_read(self, client.self);

        ClientError error = ClientError.fromValue(err);

        if (error != ClientError.OK)
            throw new ClientException("failed to read data set values", err);
    }

    @Override
    public void finalize() {
        Dispose();
    }
}
