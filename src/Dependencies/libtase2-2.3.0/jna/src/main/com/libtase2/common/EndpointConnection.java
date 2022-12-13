package com.libtase2.common;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;

/*
 *  Copyright 2019 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

/**
 * Represents a connection to another connected TASE.2 endpoint
 */
public class EndpointConnection {
    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        Pointer Tase2_Endpoint_Connection_getPeerIpAddress(Pointer self);

        Pointer Tase2_Endpoint_Connection_getPeerApTitle(Pointer self);

        int Tase2_Endpoint_Connection_getPeerAeQualifier(Pointer self);

        int Tase2_Endpoint_Connection_getMaxPduSize(Pointer self);

        Pointer Tase2_Endpoint_Connection_getPeerTSel(Pointer self);

        Pointer Tase2_Endpoint_Connection_getPeerSSel(Pointer self);

        Pointer Tase2_Endpoint_Connection_getPeerPSel(Pointer self);

        void Tase2_FreeMemory(Pointer ptr);
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

    public class Tase2_TSelector extends Structure {

        public byte size;
        public byte[] value = new byte[4];

        @Override
        protected List<String> getFieldOrder() {
            List<String> fieldOrder = new LinkedList<String>();

            fieldOrder.add("size");
            fieldOrder.add("value");

            return fieldOrder;
        }

        public Tase2_TSelector(Pointer p) {
            super(p);
            read();
        }

    }

    public class Tase2_SSelector extends Structure {

        public byte size;
        public byte[] value = new byte[16];

        @Override
        protected List<String> getFieldOrder() {
            List<String> fieldOrder = new LinkedList<String>();

            fieldOrder.add("size");
            fieldOrder.add("value");

            return fieldOrder;
        }

        public Tase2_SSelector(Pointer p) {
            super(p);
            read();
        }

    }

    public class Tase2_PSelector extends Structure {

        public byte size;
        public byte[] value = new byte[16];

        @Override
        protected List<String> getFieldOrder() {
            List<String> fieldOrder = new LinkedList<String>();

            fieldOrder.add("size");
            fieldOrder.add("value");

            return fieldOrder;
        }

        public Tase2_PSelector(Pointer p) {
            super(p);
            read();
        }
    }

    public Pointer self = Pointer.NULL;

    private String peerAddress = null;
    private String peerApTitle = null;
    private int maxPduSize = 0;

    private byte[] tSelector = null;
    private byte[] pSelector = null;
    private byte[] sSelector = null;

    EndpointConnection(Pointer self) {
        this.self = self;
    }

    /**
     * Get the address of the peer application (IP address and port number)
     *
     * @return the IP address and port number as strings separated by the ':'
     *         character.
     */
    public String getPeerIpAddress() {
        if (peerAddress == null) {
            Pointer peerAddrPtr = NativeLibTase2.INSTANCE.Tase2_Endpoint_Connection_getPeerIpAddress(self);

            if (peerAddrPtr != Pointer.NULL) {
                peerAddress = peerAddrPtr.getString(0);
            }
        }

        return peerAddress;
    }

    /**
     * Get the ISO ACSE AP-title of the peer
     *
     * @return the AP-title as string. The AP-title parts are separated by dots
     *         ('.')
     */
    public String getPeerApTitle() {
        if (peerApTitle == null) {
            Pointer apTitlePtr = NativeLibTase2.INSTANCE.Tase2_Endpoint_Connection_getPeerApTitle(self);

            if (apTitlePtr != Pointer.NULL) {
                peerApTitle = apTitlePtr.getString(0);
            }
        }

        return peerApTitle;
    }

    /**
     * Get the ISO ACSE AE-qualifier of the peer
     *
     * @return the AE-qualifier value
     */
    public int getPeerAeQualifier() {
        return NativeLibTase2.INSTANCE.Tase2_Endpoint_Connection_getPeerAeQualifier(self);
    }

    /**
     * Get the P-selector (presentation layer address) of the peer
     * 
     * @return the P-selector or null when missing
     */
    public byte[] getPSelector() {
        if (pSelector == null) {
            Pointer pSelPtr = NativeLibTase2.INSTANCE.Tase2_Endpoint_Connection_getPeerPSel(self);

            if (pSelPtr != Pointer.NULL) {
                Tase2_PSelector psel = new Tase2_PSelector(pSelPtr);

                pSelector = new byte[psel.size];

                for (int i = 0; i < psel.size; i++)
                    pSelector[i] = psel.value[i];
            }
        }

        return pSelector;
    }

    /**
     * Get the S-selector (session layer address) of the peer
     * 
     * @return the S-selector or null when missing
     */
    public byte[] getSSelector() {
        if (sSelector == null) {
            Pointer sSelPtr = NativeLibTase2.INSTANCE.Tase2_Endpoint_Connection_getPeerSSel(self);

            if (sSelPtr != Pointer.NULL) {
                Tase2_SSelector ssel = new Tase2_SSelector(sSelPtr);

                sSelector = new byte[ssel.size];

                for (int i = 0; i < ssel.size; i++)
                    sSelector[i] = ssel.value[i];
            }
        }

        return sSelector;
    }

    /**
     * Get the T-selector (transport layer address) of the peer
     * 
     * @return the T-selector or null when missing
     */
    public byte[] getTSelector() {
        if (tSelector == null) {
            Pointer tSelPtr = NativeLibTase2.INSTANCE.Tase2_Endpoint_Connection_getPeerTSel(self);

            if (tSelPtr != Pointer.NULL) {
                Tase2_TSelector tsel = new Tase2_TSelector(tSelPtr);

                tSelector = new byte[tsel.size];

                for (int i = 0; i < tsel.size; i++)
                    tSelector[i] = tsel.value[i];
            }
        }

        return tSelector;
    }

    /**
     * Get the maximum MMS PDU size used by this connection
     * 
     * @return the maximum MMS PDI size in bytes
     */
    public int getMaxPduSize() {
        return NativeLibTase2.INSTANCE.Tase2_Endpoint_Connection_getMaxPduSize(self);
    }
}
