package com.libtase2.client;

/*
 *  Copyright 2018 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import java.util.HashMap;
import java.util.Map;
import com.libtase2.common.DSConditions;
import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.TypeConverter;
import com.sun.jna.ToNativeContext;

public class ClientDSTransferSet {

    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        int Tase2_ClientDSTransferSet_readValues(Pointer self, Pointer client);

        int Tase2_ClientDSTransferSet_writeValues(Pointer self, Pointer client);

        String Tase2_ClientDSTransferSet_getDomain(Pointer self);

        String Tase2_ClientDSTransferSet_getName(Pointer self);

        String Tase2_ClientDSTransferSet_getDataSetDomain(Pointer self);

        String Tase2_ClientDSTransferSet_getDataSetName(Pointer self);

        Pointer Tase2_ClientDSTransferSet_getDataSet(Pointer self);

        int Tase2_ClientDSTransferSet_getStartTime(Pointer self);

        int Tase2_ClientDSTransferSet_getInterval(Pointer self);

        int Tase2_ClientDSTransferSet_getTLE(Pointer self);

        int Tase2_ClientDSTransferSet_getBufferTime(Pointer self);

        int Tase2_ClientDSTransferSet_getIntegrityCheck(Pointer self);

        int Tase2_ClientDSTransferSet_getDSConditionsRequested(Pointer self);

        boolean Tase2_ClientDSTransferSet_getBlockData(Pointer self);

        boolean Tase2_ClientDSTransferSet_getCritical(Pointer self);

        boolean Tase2_ClientDSTransferSet_getRBE(Pointer self);

        boolean Tase2_ClientDSTransferSet_getAllChangesReported(Pointer self);

        boolean Tase2_ClientDSTransferSet_getStatus(Pointer self);

        short Tase2_ClientDSTransferSet_getEventCodeRequested(Pointer self);

        void Tase2_ClientDSTransferSet_setDataSetName(Pointer self, String domainId, String dataSetId);

        void Tase2_ClientDSTransferSet_setDataSet(Pointer self, Pointer dataSet);

        void Tase2_ClientDSTransferSet_setStartTime(Pointer self, int startTime);

        void Tase2_ClientDSTransferSet_setInterval(Pointer self, int interval);

        void Tase2_ClientDSTransferSet_setTLE(Pointer self, int tle);

        void Tase2_ClientDSTransferSet_setBufferTime(Pointer self, int bufferTime);

        void Tase2_ClientDSTransferSet_setIntegrityCheck(Pointer self, int integrityCheck);

        void Tase2_ClientDSTransferSet_setDSConditionsRequested(Pointer self, int dsConditionsRequested);

        void Tase2_ClientDSTransferSet_setBlockData(Pointer self, boolean blockData);

        void Tase2_ClientDSTransferSet_setCritical(Pointer self, boolean critical);

        void Tase2_ClientDSTransferSet_setRBE(Pointer self, boolean rbe);

        void Tase2_ClientDSTransferSet_setAllChangesReported(Pointer self, boolean allChangesReported);

        void Tase2_ClientDSTransferSet_setStatus(Pointer self, boolean status);

        int Tase2_ClientDSTransferSet_writeDataSetName(Pointer self, Pointer client, String domainId, String dataSetId);

        int Tase2_ClientDSTransferSet_writeStartTime(Pointer self, Pointer client, int startTime);

        int Tase2_ClientDSTransferSet_writeInterval(Pointer self, Pointer client, int interval);

        int Tase2_ClientDSTransferSet_writeTLE(Pointer self, Pointer client, int tle);

        int Tase2_ClientDSTransferSet_writeBufferTime(Pointer self, Pointer client, int bufferTime);

        int Tase2_ClientDSTransferSet_writeIntegrityCheck(Pointer self, Pointer client, int integrityCheck);

        int Tase2_ClientDSTransferSet_writeDSConditionsRequested(Pointer self, Pointer client,
                int dsConditionsRequested);

        int Tase2_ClientDSTransferSet_writeBlockData(Pointer self, Pointer client, boolean blockData);

        int Tase2_ClientDSTransferSet_writeCritical(Pointer self, Pointer client, boolean critical);

        int Tase2_ClientDSTransferSet_writeRBE(Pointer self, Pointer client, boolean rbe);

        int Tase2_ClientDSTransferSet_writeAllChangesReported(Pointer self, Pointer client, boolean allChangesReported);

        int Tase2_ClientDSTransferSet_writeStatus(Pointer self, Pointer client, boolean status);

        byte Tase2_ClientDSTransferSet_getDSConditionsDetected(Pointer self);

        short Tase2_ClientDSTransferSet_getEventCodeDetected(Pointer self);

        int Tase2_ClientDSTransferSet_getTransferSetTimestamp(Pointer self);
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

    private ClientDataSet dataSet = null;

    public ClientDSTransferSet(Pointer self) {
        this.self = self;
    }

    /**
     * Read the values of the DSTransferSet from the server
     * 
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     * 
     * @param client the connection to use
     * @throws ClientException
     */
    public void readValues(Client client) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_readValues(self, client.self);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to read DSTS values", err);
    }

    /**
     * Write the buffered values of the DSTransferSet to the server
     * 
     * <p>
     * NOTE: This function can only be used after calling the
     * {@link #readValues(Client)} method to get the current values of the transfer
     * set in the server.
     * </p>
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     * 
     * @param client the connection to use
     * 
     * @throws ClientException
     */
    public void writeValues(Client client) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeValues(self, client.self);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS values", err);
    }

    /**
     * Get the domain name of the transfer set
     * 
     * @return the domain name or null when the transfer set is in VCC scope
     */
    public String getDomain() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getDomain(self);
    }

    /**
     * Get the name of the transfer set
     * 
     * @return the name of the transfer set
     */
    public String getName() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getName(self);
    }

    /**
     * Get the domain name of the transfer set data set
     * 
     * @return the domain name or null when the data set is in VCC scope
     */
    public String getDataSetDomain() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getDataSetDomain(self);
    }

    /**
     * Get the name of the transfer set data set
     * 
     * @return the name of the data set
     */
    public String getDataSetName() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getDataSetName(self);
    }

    /**
     * Get {@link ClientDataSet} instance of the transfer set data set
     *
     * @return the {@link ClientDataSet} instance of the transfer set
     */
    public ClientDataSet getDataSet() {
        Pointer dataSetPtr = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getDataSet(self);

        if (dataSetPtr.equals(Pointer.NULL))
            return null;
        else
            return new ClientDataSet(dataSetPtr);
    }

    /**
     * Get the start time of report transmission
     * 
     * <p>
     * This is the start time for the report interval
     * </p>
     *
     * @return the transmission start time value in seconds since 01.01.1970 UTC
     */
    public int getStartTime() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getStartTime(self);
    }

    /**
     * Get the transmission interval of the report
     * 
     * <p>
     * When the report time is set, then events are collected and reported after the
     * interval timeout.
     * </p>
     *
     * @return transmission interval for periodic transmission in seconds
     */
    public int getInterval() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getInterval(self);
    }

    /**
     * Get the TLE (Time Limit for Execution) value
     *
     * @return TLE value in seconds
     */
    public int getTLE() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getTLE(self);
    }

    /**
     * Get the buffer time value
     * 
     * <p>
     * The buffer time is the time between an object change and the transmission of
     * the report. During the buffer time events are collected and can be
     * transmitted with the same report. If the buffer time is 0 then every event
     * causes a report.
     * </p>
     *
     * @return buffer time value in seconds
     */
    public int getBufferTime() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getBufferTime(self);
    }

    /**
     * Get the integrity check interval time
     *
     * @return the integrity check interval time value in seconds
     */
    public int getIntegrityCheck() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getIntegrityCheck(self);
    }

    /**
     * Get the DSConditions to trigger a report transmission
     *
     * @return the DSConditions value (\ref ds_conditions)
     */
    public int getDSConditionsRequested() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getDSConditionsRequested(self);
    }

    /**
     * Get the information if blocked data transfer is used
     * 
     * <p>
     * NOTE: blocked data transfer (CB3) is not supported!
     * </p>
     *
     * @return true when blocked data transfer is used, false otherwise
     */
    public boolean getBlockData() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getBlockData(self);
    }

    /**
     * Get the value of the critical flag
     *
     * <p>
     * The critical flag indicates that the client has to confirm the reception of
     * the critical report.
     * </p>
     *
     * @return true if critical is set, false otherwise
     */
    public boolean getCritical() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getCritical(self);
    }

    /**
     * Get the value of the RBE (report by exception) flag
     *
     * <p>
     * When RBE is activated only the changed objects are reported. Otherwise all
     * objects in the transfer set data set are reported.
     * </p>
     *
     * @return true if RBE is enabled, false otherwise
     */
    public boolean getRBE() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getRBE(self);
    }

    /**
     * Get the value of the AllChangesReported attribute
     *
     * <p>
     * This attributes indicates if all changes of an element during the buffer time
     * are reported (true), or only the last change (false).
     * </p>
     *
     * @return true if all changes reported is enabled, false otherwise
     */
    public boolean getAllChangesReported() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getAllChangesReported(self);
    }

    /**
     * Get status of the transfer set
     *
     * @return true when the transfer set (reporting) is enabled, false otherwise
     */
    public boolean getStatus() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getStatus(self);
    }

    /**
     * Get the external event code that caused the transmission
     *
     * @return the external event code
     */
    public short getEventCodeRequested() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getEventCodeRequested(self);
    }

    /**
     * Sets the name of the transfer set data set
     *
     * @param domainId  domain name of the data set (null for VCC scope data set)
     * @param dataSetid name of the data set
     */
    public void setDataSetName(String domainId, String dataSetId) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setDataSetName(self, domainId, dataSetId);
    }

    /**
     * Associate a {@link ClientDataSet} instance with the transfer set
     *
     * @param dataSet the {@link ClientDataSet} instance to use
     */
    public void setDataSet(ClientDataSet dataSet) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setDataSet(self, dataSet.self);
        this.dataSet = dataSet;
    }

    /**
     * Sets the start time of transfer set monitoring
     * 
     * <p>
     * This is the start time for the report interval
     * </p>
     *
     * @param startTime Start time as seconds since epoch.
     */
    public void setStartTime(int startTime) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setStartTime(self, startTime);
    }

    /**
     * Sets the interval time for transfer set reports
     *
     * <p>
     * The interval time determines the time period for cyclic report transmission.
     * </p>
     *
     * @param interval the interval time in seconds.
     */
    public void setInterval(int interval) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setInterval(self, interval);
    }

    /**
     * Sets the TLE (Time Limit for Execution) value.
     *
     * @param tle TLE value in seconds
     */
    public void setTLE(int tle) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setTLE(self, tle);
    }

    /**
     * Sets the buffer time (wait time until report is sent)
     *
     * @param bufferTime the buffer time value in seconds
     */
    public void setBufferTime(int bufferTime) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setBufferTime(self, bufferTime);
    }

    /**
     * Sets the integrity check interval time
     *
     * @param integrityCheck the integrity check value in seconds.
     */
    public void setIntegrityCheck(int integrityCheck) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setIntegrityCheck(self, integrityCheck);
    }

    /**
     * Sets the trigger conditions for reports
     *
     * @param dsConditionRequested the trigger conditions (bitmask/sum of
     *                             {@link DSConditions} values)
     */
    public void setDSConditionsRequested(int dsConditionsRequested) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setDSConditionsRequested(self, dsConditionsRequested);
    }

    /**
     * Enables/disables block data transfer
     *
     * <p>
     * The block data mode provides a compressed data format for transmission to
     * save bandwidth. NOTE: Should not be set to true! Block data transfer (CB3 is
     * not supported by the library)
     * </p>
     *
     * @param value true to enable block data transfer, false otherwise
     */
    public void setBlockData(boolean value) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setBlockData(self, value);
    }

    /**
     * Sets the critical flag.
     *
     * <p>
     * NOTE: critical reports have to be acknowledged by the client.
     * </p>
     *
     * @param value true, enable critical transfer report, false, otherwise
     */
    public void setCritical(boolean value) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setCritical(self, value);
    }

    /**
     * Set the value of the RBE (report by exception) flag
     *
     * @param value true, enable RBE, false, otherwise
     */
    public void setRBE(boolean value) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setRBE(self, value);
    }

    /**
     * Enable/disable transmission of all changes
     * 
     * @param value when true, all changes of an object are reported, when false,
     *              changes that happen in the buffer time may not be reported
     */
    public void setAllChangesReported(boolean value) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setAllChangesReported(self, value);
    }

    /**
     * Enable/disable the status of condition monitoring (and report transmission)
     *
     * @param value true, enable reporting, false otherwise
     */
    public void setStatus(boolean value) {
        NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_setStatus(self, value);
    }

    /**
     * Write the name of the transfer set data set (write immediately to server)
     * 
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client    the related client instance used to send the write message
     *                  to the server
     * @param domainId  domain name of the data set
     * @param dataSetid name of the data set
     * @throws ClientException
     */
    public void writeDataSetName(Client client, String domainName, String dataSetName) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeDataSetName(self, client.self, domainName,
                dataSetName);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.DataSetName", err);
    }

    /**
     * Sets the start time of transfer set monitoring (write immediately to server)
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the related client instance used to send the write message to
     *               the server
     * @param value  Start time as seconds since epoch.
     * @throws ClientException
     */
    public void writeStartTime(Client client, int value) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeStartTime(self, client.self, value);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.StartTime", err);
    }

    /**
     * Sets the interval time for transfer set reports (write immediately to server)
     *
     * <p>
     * The interval time determines the time period for cyclic report transmission.
     * </p>
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the related client instance used to send the write message to
     *               the server
     * @param value  the interval time in seconds.
     * @throws ClientException
     */
    public void writeInterval(Client client, int value) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeInterval(self, client.self, value);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.Interval", err);
    }

    /**
     * Sets the TLE (Time Limit for Execution) value (write immediately to server).
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the related client instance used to send the write message to
     *               the server
     * @param value  TLE value in seconds
     * @throws ClientException
     */
    public void writeTLE(Client client, int value) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeTLE(self, client.self, value);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.TLE", err);
    }

    /**
     * Sets the buffer time (wait time until report is sent) (write immediately to
     * server)
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the related client instance used to send the write message to
     *               the server
     * @param value  the buffer time value in seconds
     * @throws ClientException
     */
    public void writeBufferTime(Client client, int value) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeBufferTime(self, client.self, value);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.BufferTime", err);
    }

    /**
     * Sets the integrity check interval time (write immediately to server)
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the related client instance used to send the write message to
     *               the server
     * @param value  the integrity check value in seconds.
     * @throws ClientException
     */
    public void writeIntegrityCheck(Client client, int value) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeIntegrityCheck(self, client.self, value);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.IntegrityCheck", err);
    }

    /**
     * Sets the trigger conditions for reports ({@link DSConditions}) (write
     * immediately to server)
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the related client instance used to send the write message to
     *               the server
     * @param value  the trigger conditions (bitmask/sum of {@link DSConditions})
     * @throws ClientException
     */
    public void writeDSConditionsRequested(Client client, int value) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeDSConditionsRequested(self, client.self,
                value);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.DSConditionsRequested", err);
    }

    /**
     * Enables/disables block data transfer (write immediately to server)
     *
     * <p>
     * The block data mode provides a compressed data format for transmission to
     * save bandwidth. NOTE: Should not be set to true! Block data transfer (CB3 is
     * not supported by the library)
     * </p>
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the related client instance used to send the write message to
     *               the server
     * @param value  true to enable block data transfer, false otherwise
     * @throws ClientException
     */
    public void writeBlockData(Client client, boolean value) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeBlockData(self, client.self, value);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.BlockData", err);
    }

    /**
     * Set the value of the critical flag (write immediately to server)
     *
     * <p>
     * The critical flag indicates that the client has to confirm the reception of
     * the critical report.
     * </p>
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the related client instance used to send the write message to
     *               the server
     * @param value  true to set the critical flag, false otherwise
     * @throws ClientException
     */
    public void writeCritical(Client client, boolean value) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeCritical(self, client.self, value);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.Critical", err);
    }

    /**
     * Write the value of the RBE (report by exception) flag (write immediately to
     * server)
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the related client instance used to send the write message to
     *               the server
     * @param value  true, send reports when a value changes
     * @throws ClientException
     */
    public void writeRBE(Client client, boolean value) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeRBE(self, client.self, value);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.RBE", err);
    }

    /**
     * Enable/disable transmission of all changes (write immediately to server)
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the related client instance used to send the write message to
     *               the server
     * @param value  when true, all changes are reported, when false, changes that
     *               happen in the buffer time may not be reported
     * @throws ClientException
     */
    public void writeAllChangesReported(Client client, boolean value) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeAllChangesReported(self, client.self, value);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.AllChangesReported", err);
    }

    /**
     * Enable/disable the status of condition monitoring and report transmission
     * (write immediately to server)
     *
     * <p>
     * DO NOT CALL THIS FUNCTION INSIDE A LIBRARY CALLBACK! Calling this function
     * inside library callbacks can cause deadlock situations.
     * </p>
     *
     * @param client the related client instance used to send the write message to
     *               the server
     * @param value  true, enable reporting, false otherwise
     * @throws ClientException
     */
    public void writeStatus(Client client, boolean value) throws ClientException {
        int err = NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_writeStatus(self, client.self, value);

        ClientError clientErr = ClientError.fromValue(err);

        if (clientErr != ClientError.OK)
            throw new ClientException("Failed to write DSTS.Status", err);
    }

    /**
     * Get the DSConditionsDetected value from the last received report
     * 
     * <p>
     * This represents an optional special data set variable. When part of the
     * transfer set data set this will be sent with reports.
     * </p>
     *
     * <p>
     * At the time of reporting, this attribute uniquely identifies the conditions
     * that caused the generation of the report.
     * </p>
     *
     * <p>
     * NOTE: The value may not be present in a report.
     * </p>
     * 
     * @return Detected value (reason code for transfer report)
     */
    public short getDSConditionsDetected() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getDSConditionsDetected(self);
    }

    /**
     * Get the event code detected value from the last received report
     *
     * <p>
     * This represents an optional special data set variable. When part of the
     * transfer set data set this will be sent with reports.
     * </p>
     *
     * @return the event code
     */
    public int getEventCodeDetected() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getEventCodeDetected(self);
    }

    /**
     * @brief Get the transfer set timestamp value from the last received report
     *
     *        <p>
     *        This represents an optional special data set variable. When part of
     *        the transfer set data set this will be sent with reports.
     *        </p>
     *
     *        <p>
     *        This attribute specifies the time the transfer report was generated at
     *        the server.
     *        </p>
     *
     * @return transfer set timestamp value in seconds since 01.01.1970 UTC
     */
    public int getTransferSetTimestamp() {
        return NativeLibTase2.INSTANCE.Tase2_ClientDSTransferSet_getTransferSetTimestamp(self);
    }

}
