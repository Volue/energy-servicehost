package com.libtase2.server;

import java.util.HashMap;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;

/**
 * Specifies the data model of a {@link com.libtase2.server.Server Server}
 * instance.
 */
public class DataModel {
    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        Pointer Tase2_DataModel_create();

        void Tase2_DataModel_destroy(Pointer self);

        Pointer Tase2_DataModel_addDomain(Pointer self, String domainName);

        Pointer Tase2_DataModel_getDomain(Pointer self, String domainName);

        Pointer Tase2_DataModel_getVCC(Pointer self);

        void Tase2_DataModel_setEdition(Pointer self, int edition);
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

    private List<Domain> domainRefs = new LinkedList<Domain>();
    private Domain vcc = null;

    Pointer self;

    DataPoint getDataPointByPointer(Pointer ptr) {
        DataPoint dp = null;

        getVCC();

        if (vcc != null)
            dp = vcc.getDataPointByPtr(ptr);

        if (dp == null) {

            for (Domain domain : domainRefs) {

                dp = domain.getDataPointByPtr(ptr);

                if (dp != null)
                    break;
            }
        }

        return dp;
    }

    DSTransferSet getDSTransferSetByPtr(Pointer ptr) {
        DSTransferSet ts = null;

        getVCC();

        if (vcc != null)
            ts = vcc.getDSTransferSetByPtr(ptr);

        if (ts == null) {
            for (Domain domain : domainRefs) {
                ts = domain.getDSTransferSetByPtr(ptr);

                if (ts != null)
                    break;
            }
        }

        return ts;
    }

    /**
     * Create a new data model instance
     *
     * NOTE: a data model can only be used by a single
     * {@link com.libtase2.server.Server Server} instance!
     */
    public DataModel() {
        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_DataModel_create();

        if (ptr != Pointer.NULL)
            this.self = ptr;
        else
            throw new RuntimeException("Failed to create new instance");
    }

    public void setEdition(Edition edition) {
        NativeLibTase2.INSTANCE.Tase2_DataModel_setEdition(this.self, edition.getValue());
    }

    /**
     * Add a TASE.2 domain to the data model
     *
     * @param domainName the name of the domain (max. 32 characters)
     *
     * @return the new domain instance
     */
    public Domain addDomain(String domainName) {
        Pointer domainPtr = NativeLibTase2.INSTANCE.Tase2_DataModel_addDomain(self, domainName);

        if (domainPtr == Pointer.NULL)
            throw new RuntimeException("Failed to create new domain instance");

        Domain domain = new Domain(domainPtr, domainName);

        domainRefs.add(domain);

        return domain;
    }

    /**
     * Get the domain instance with a specific name
     *
     * @param domainName the domain name
     *
     * @return the domain with the specified name
     */
    public Domain getDomain(String domainName) {
        Domain domain = null;

        for (Domain dom : domainRefs) {
            if (dom.getName().equals(domainName)) {
                domain = dom;
                break;
            }
        }

        if (domain == null) {
            Pointer domainPtr = NativeLibTase2.INSTANCE.Tase2_DataModel_getDomain(self, domainName);

            if (domainPtr != Pointer.NULL)
                domain = new Domain(domainPtr, domainName);
        }

        return domain;
    }

    Domain getDomainByPtr(Pointer ptr) {
        Domain domain = null;

        for (Domain dom : domainRefs) {
            if (dom.self.equals(ptr)) {
                domain = dom;
                break;
            }
        }

        return domain;
    }

    /**
     * Get the VCC pseudo domain that is representing the VCC scope data model
     * items.
     *
     * @return domain object the is representing the VCC scope
     */
    public Domain getVCC() {
        if (vcc == null) {
            Pointer vccPtr = NativeLibTase2.INSTANCE.Tase2_DataModel_getVCC(self);

            if (vccPtr != Pointer.NULL)
                vcc = new Domain(vccPtr, null);
        }

        return vcc;
    }

    /**
     * Release all resources and memory used by the data model
     */
    public void dispose() {
        if (self != null) {
            NativeLibTase2.INSTANCE.Tase2_DataModel_destroy(self);
            self = null;
        }
    }

    @Override
    protected void finalize() {
        dispose();
    }
}
