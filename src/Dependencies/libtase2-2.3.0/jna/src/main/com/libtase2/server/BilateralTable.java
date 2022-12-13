package com.libtase2.server;

import java.lang.ref.WeakReference;
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

/**
 * The bilateral table (BLT) defines the domain and access rights of a peer
 * (client)
 */
public class BilateralTable {
    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        public static class Tase2_TSelector extends Structure {

            public static class ByValue extends Tase2_TSelector implements Structure.ByValue {
            }

            public byte size;
            public byte[] value = new byte[4];

            @Override
            protected List<String> getFieldOrder() {
                List<String> fieldOrder = new LinkedList<String>();

                fieldOrder.add("size");
                fieldOrder.add("value");

                return fieldOrder;
            }

        }

        public static class Tase2_SSelector extends Structure {

            public static class ByValue extends Tase2_SSelector implements Structure.ByValue {
            }

            public byte size;
            public byte[] value = new byte[16];

            @Override
            protected List<String> getFieldOrder() {
                List<String> fieldOrder = new LinkedList<String>();

                fieldOrder.add("size");
                fieldOrder.add("value");

                return fieldOrder;
            }

        }

        public static class Tase2_PSelector extends Structure {

            public static class ByValue extends Tase2_PSelector implements Structure.ByValue {
            }

            public byte size;
            public byte[] value = new byte[16];

            @Override
            protected List<String> getFieldOrder() {
                List<String> fieldOrder = new LinkedList<String>();

                fieldOrder.add("size");
                fieldOrder.add("value");

                return fieldOrder;
            }

        }

        Pointer Tase2_BilateralTable_create(String bltId, Pointer domain, String apTitle, int aeQualifier);

        void Tase2_BilateralTable_setIsoParameters(Pointer self, Tase2_PSelector.ByValue pSelector,
                Tase2_SSelector.ByValue sSelector, Tase2_TSelector.ByValue tSelector);

        void Tase2_BilateralTable_addDataPoint(Pointer self, Pointer dataPoint, boolean allowRead, boolean allowWrite);

        void Tase2_BilateralTable_addControlPoint(Pointer self, Pointer controlPoint, short checkBackId,
                boolean allowSetTag, boolean allowGetTag, boolean allowOperate, boolean allowRead);

        void Tase2_BilateralTable_addInformationMessage(Pointer self, int infoRef, int localRef, boolean vccScope);

        String Tase2_BilateralTable_getID(Pointer self);
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

    private static List<WeakReference<BilateralTable>> managedInstances = new LinkedList<WeakReference<BilateralTable>>();

    private static void cleanupManagedInstanceList() {
        List<WeakReference<BilateralTable>> deadInstances = new LinkedList<WeakReference<BilateralTable>>();

        synchronized (managedInstances) {
            for (WeakReference<BilateralTable> bltRef : managedInstances) {

                BilateralTable blt = bltRef.get();

                if (blt == null) {
                    deadInstances.add(bltRef);
                }
            }

            managedInstances.removeAll(deadInstances);
        }
    }

    protected static BilateralTable getManagedInstance(Pointer ptr) {
        BilateralTable instance = null;

        synchronized (managedInstances) {

            for (WeakReference<BilateralTable> bltRef : managedInstances) {

                BilateralTable blt = bltRef.get();

                if (blt != null) {
                    if (blt.self.equals(ptr)) {
                        instance = blt;
                        break;
                    }
                }
            }
        }

        return instance;
    }

    protected Pointer self;

    private String apTitle = null;
    private int aeQualifier = 0;

    /**
     * Create a new bilateral table (\ref Tase2_BilateralTable) instance
     *
     * NOTE: the bilateral table is associated with a client by the application
     * reference (ap-title + ae-qualifier)
     *
     * @param bltId       The bilateral table ID (should be unique). Can be accessed
     *                    from the client by the "Bilateral_Table_ID" variable
     * @param domain      The {@link com.libtase2.server.Domain domain} that is
     *                    associated with the client (only the associated client can
     *                    access this domain)
     * @param apTitle     the ap-title as string (point separated values e.g.
     *                    "1.1.1.999")
     * @param aeQualifier the ae-qualifier value
     */
    public BilateralTable(String bltId, Domain domain, String apTitle, int aeQualifier) {
        self = NativeLibTase2.INSTANCE.Tase2_BilateralTable_create(bltId, domain.self, apTitle, aeQualifier);

        this.apTitle = apTitle;
        this.aeQualifier = aeQualifier;

        synchronized (managedInstances) {
            managedInstances.add(new WeakReference<BilateralTable>(this));
        }
    }

    @Override
    protected void finalize() throws Throwable {
        synchronized (managedInstances) {
            WeakReference<BilateralTable> instanceToRemove = null;

            for (WeakReference<BilateralTable> bltRef : managedInstances) {

                BilateralTable blt = bltRef.get();

                if (blt == this) {
                    instanceToRemove = bltRef;
                    break;
                }
            }

            managedInstances.remove(instanceToRemove);
        }
    }

    /**
     * Set the ISO protocol layer addresses to associate the endpoint to this BLT
     *
     * NOTE: By default only AP-title and AE-qualifier are used to associate an
     * endpoint with the BLT. You can change this behavior with the
     * {@link com.libtase2.common.Endpoint#setAddressMatchingRules(boolean, boolean, boolean, boolean, boolean)
     * Endpoint.setAddressMathingRules} function. When using the ISO layer addresses
     * you have to set them with this function.
     *
     * @param pSelector presentation layer address
     * @param sSelector session layer address
     * @param tSelector ISO COTP layer address
     */
    public void setIsoParameters(byte[] pSelector, byte[] sSelector, byte[] tSelector) {
        if (pSelector.length > 16)
            throw new IllegalArgumentException("p-selector too long (max 16 bytes)!");

        if (sSelector.length > 16)
            throw new IllegalArgumentException("s-selector too long (max 16 bytes)!");

        if (tSelector.length > 16)
            throw new IllegalArgumentException("s-selector too long (max 16 bytes)!");

        NativeLibTase2.Tase2_PSelector.ByValue pSel = new NativeLibTase2.Tase2_PSelector.ByValue();
        pSel.size = (byte) pSelector.length;
        for (int i = 0; i < pSel.size; i++)
            pSel.value[i] = pSelector[i];

        NativeLibTase2.Tase2_SSelector.ByValue sSel = new NativeLibTase2.Tase2_SSelector.ByValue();
        sSel.size = (byte) sSelector.length;
        for (int i = 0; i < sSel.size; i++)
            sSel.value[i] = sSelector[i];

        NativeLibTase2.Tase2_TSelector.ByValue tSel = new NativeLibTase2.Tase2_TSelector.ByValue();
        tSel.size = (byte) tSelector.length;
        for (int i = 0; i < tSel.size; i++)
            tSel.value[i] = tSelector[i];

        NativeLibTase2.INSTANCE.Tase2_BilateralTable_setIsoParameters(self, pSel, sSel, tSel);
    }

    /**
     * Add an indication point or protection equipment object to the BLT
     *
     * @param dataPoint  the indication point or protection equipment object
     * @param allowRead  allow read operation on this data point
     * @param allowWrite allow write operation on this data point
     */
    public void addDataPoint(DataPoint dataPoint, boolean allowRead, boolean allowWrite) {
        NativeLibTase2.INSTANCE.Tase2_BilateralTable_addDataPoint(self, dataPoint.self, allowRead, allowWrite);
    }

    /**
     * Add a control point object to the BLT
     *
     * @param controlPoint the control point instance
     * @param checkBackId  the check back ID used as SBO select return value
     * @param allowSetTag  allow set tag operation on this control point
     * @param allowGetTag  allow get tag operation on this control point
     * @param allowOperate allow operate operation on this control point
     * @param allowRead    allow read operation on this control point
     */
    public void addControlPoint(ControlPoint controlPoint, short checkBackId, boolean allowSetTag, boolean allowGetTag,
            boolean allowOperate, boolean allowRead) {
        NativeLibTase2.INSTANCE.Tase2_BilateralTable_addControlPoint(self, controlPoint.self, checkBackId, allowSetTag,
                allowGetTag, allowOperate, allowRead);
    }

    /**
     * Add an information message object to the BLT
     *
     * @param self     The Tase2_BilateralTable instance
     * @param infoRef  information reference parameter (specifies application)
     * @param localRef local reference parameter
     * @param vccScope indicates if object has VCC scope (true) or ICC scope (false)
     */
    public void addInformationMessage(int infoRef, int localRef, boolean vccScope) {
        NativeLibTase2.INSTANCE.Tase2_BilateralTable_addInformationMessage(self, infoRef, localRef, vccScope);
    }

    /**
     * Get the BLT ID
     */
    public String getID() {
        return NativeLibTase2.INSTANCE.Tase2_BilateralTable_getID(self);
    }

    /**
     * Get the AP-title
     * 
     * @return the ap-title as string (point separated values e.g. "1.1.1.999")
     */
    public String getApTitle() {
        return apTitle;
    }

    /**
     * Get the AE-qualifier
     * 
     * @return the ae-qualifier value
     */
    public int getAeQualifier() {
        return aeQualifier;
    }
}
