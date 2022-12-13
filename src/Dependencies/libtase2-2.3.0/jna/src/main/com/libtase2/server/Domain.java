package com.libtase2.server;

import java.util.HashMap;
import java.util.Map;

import com.libtase2.common.TimeStampClass;
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
 * Represents a TASE.2 domain (usually a domain is dedicated to a specific
 * control center connection)
 */
public class Domain {
    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        void Tase2_Domain_setBilateralTableId(Pointer self, String id);

        Pointer Tase2_Domain_addIndicationPoint(Pointer self, String name, int type, int qualityClass,
                int timeStampClass, byte hasCOV, byte readOnly);

        Pointer Tase2_Domain_addControlPoint(Pointer self, String deviceName, int pointType, int deviceClass,
                byte hasTag, short checkBackId);

        Pointer Tase2_Domain_addProtectionEquipment(Pointer self, String equipmentName, byte isPacked);

        Pointer Tase2_Domain_addDSTransferSet(Pointer self, String name);

        Pointer Tase2_Domain_addDataSet(Pointer self, String name);

        Pointer Tase2_Domain_getDataPoint(Pointer self, String pointName);

        void Tase2_Domain_addInformationMessageObject(Pointer self, int maxSize);
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

    private HashMap<Pointer, DataPoint> dataPoints = new HashMap<Pointer, DataPoint>();

    private HashMap<Pointer, DataSet> dataSets = new HashMap<Pointer, DataSet>();

    private HashMap<Pointer, DSTransferSet> transferSets = new HashMap<Pointer, DSTransferSet>();

    Domain(Pointer domainPtr, String domainName) {
        self = domainPtr;
        name = domainName;
    }

    DataPoint getDataPointByPtr(Pointer ptr) {
        return dataPoints.get(ptr);
    }

    DataSet getDataSetByPtr(Pointer ptr) {
        return dataSets.get(ptr);
    }

    DSTransferSet getDSTransferSetByPtr(Pointer ptr) {
        return transferSets.get(ptr);
    }

    /**
     * Get the domain name
     * 
     * @return the domain name
     */
    public String getName() {
        return name;
    }

    /**
     * Set the bilateral table (BLT) id of the domain
     *
     * NOTE: a domain is related to a specific bilateral table that is an agreement
     * between client and server on what data is accessible by the client. Usually a
     * (server) domain is dedicated to a specific other control center (client).
     *
     * @param id the BLT ID
     */
    public void setBilateralTableId(String id) {
        NativeLibTase2.INSTANCE.Tase2_Domain_setBilateralTableId(self, id);
    }

    /**
     * Create a new data point of type indication point
     *
     * NOTE: An indication point represents measurement or status data.
     *
     * @param name           the name of the data point
     * @param type           the type of the indication point
     * @param qualityClass   defines if a quality flag is available
     * @param timeStampClass defines if and what kind of time stamp is available
     * @param hasCOV         has a COV counter
     * @param readOnly       true when indication point is read-only, false when
     *                       read-write.
     *
     * @return the new IndicationPoint instance
     */
    public IndicationPoint addIndicationPoint(String name, IndicationPointType type, QualityClass qualityClass,
            TimeStampClass timeStampClass, boolean hasCOV, boolean readOnly) {
        IndicationPoint point = null;

        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_Domain_addIndicationPoint(self, name, type.getValue(),
                qualityClass.getValue(), timeStampClass.getValue(), (byte) (hasCOV ? 1 : 0), (byte) (readOnly ? 1 : 0));

        if (ptr != Pointer.NULL) {
            point = new IndicationPoint(ptr, name, this);
            dataPoints.put(ptr, point);
        }

        return point;
    }

    /**
     * Create a new data point of type control point (device)
     *
     * NOTE: A control point (device) is used to control outputs or set points.
     *
     * @param deviceName  name of the control point
     * @param pointType   type of the control point (one of COMMAND, SETPOINT_REAL,
     *                    SETPOINT_DISCRETE)
     * @param deviceClass the device class (one of TASE2_DEVICE_CLASS_SBO or
     *                    TASE2_DEVICE_CLASS_DIRECT)
     * @param hasTag      define if the device has a tag (a tag is a restriction
     *                    that prohibits specific usage of the device)
     * @param checkBackId Set the check back ID for SBO.
     *
     * @return the new control point instance
     */
    public ControlPoint addControlPoint(String deviceName, ControlPointType pointType, DeviceClass deviceClass,
            boolean hasTag, short checkBackId) {
        ControlPoint point = null;

        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_Domain_addControlPoint(self, deviceName, pointType.getValue(),
                deviceClass.getValue(), (byte) (hasTag ? 1 : 0), checkBackId);

        if (ptr != Pointer.NULL) {

            point = new ControlPoint(ptr, deviceName, this);

            dataPoints.put(ptr, point);
        } else {
            System.err.println("Failed to create native ControlPoint");
        }

        return point;
    }

    /**
     * Create a new data point of type protection equipment
     *
     * @param equipmentName the name of the data point
     * @param isPacked      specify if packed type is used
     *
     * @return the new protection equipment instance
     */
    public ProtectionEquipment addProtectionEquipment(String equipmentName, boolean isPacked) {
        ProtectionEquipment point = null;

        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_Domain_addProtectionEquipment(self, equipmentName,
                (byte) (isPacked ? 1 : 0));

        if (ptr != Pointer.NULL) {
            point = new ProtectionEquipment(ptr, equipmentName, this);
            dataPoints.put(ptr, point);
        }

        return point;
    }

    /**
     * Add a new static data set to the domain
     * 
     * @param name the name of the data set
     * @return the new data set instance (can be used to add data set entries)
     */
    public DataSet addDataSet(String name) {
        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_Domain_addDataSet(self, name);

        if (ptr != Pointer.NULL) {
            DataSet ds = new DataSet(ptr, name);

            dataSets.put(ptr, ds);

            return ds;
        } else
            return null;
    }

    /**
     * Add a new data set transfer set (DSTS) to the domain
     *
     * @param name the name of the DSTS
     *
     * @return the new DSTS instance
     */
    public DSTransferSet addDSTransferSet(String name) {
        Pointer ptr = NativeLibTase2.INSTANCE.Tase2_Domain_addDSTransferSet(self, name);

        if (ptr != Pointer.NULL) {
            DSTransferSet transferSet = new DSTransferSet(ptr, name);

            transferSets.put(ptr, transferSet);

            return transferSet;
        } else
            return null;
    }

    /**
     * Add an information message object to a domain of VCC scope
     *
     * @param self    the domain instance or VCC pseudo domain
     * @param maxSize maximum size of the information message buffer
     */
    public void addInformationMessageObject(int size) {
        NativeLibTase2.INSTANCE.Tase2_Domain_addInformationMessageObject(self, size);
    }

}
