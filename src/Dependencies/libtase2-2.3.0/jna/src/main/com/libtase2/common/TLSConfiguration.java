package com.libtase2.common;

/*
 *  Copyright 2019-2021 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

import java.security.PrivateKey;
import java.security.cert.CRLException;
import java.security.cert.CertificateEncodingException;
import java.security.cert.X509CRL;
import java.security.cert.X509Certificate;
import java.util.HashMap;
import java.util.Map;

import com.sun.jna.DefaultTypeMapper;
import com.sun.jna.FromNativeContext;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ToNativeContext;
import com.sun.jna.TypeConverter;

/**
 * Represent the TLS configuration, keys, and certificates for an
 * {@link com.libtase2.common.Endpoint Endpoint}.
 */
public class TLSConfiguration {

    private interface NativeLibTase2 extends Library {
        NativeLibTase2 INSTANCE = (NativeLibTase2) Native.loadLibrary("tase2", NativeLibTase2.class, getOptions());

        Pointer TLSConfiguration_create();

        void TLSConfiguration_setChainValidation(Pointer self, boolean value);

        void TLSConfiguration_setAllowOnlyKnownCertificates(Pointer self, boolean value);

        boolean TLSConfiguration_setOwnCertificate(Pointer self, byte[] certificate, int certLen);

        boolean TLSConfiguration_setOwnCertificateFromFile(Pointer self, String filename);

        boolean TLSConfiguration_setOwnKey(Pointer self, byte[] key, int keyLen, String keyPassword);

        boolean TLSConfiguration_setOwnKeyFromFile(Pointer self, String filename, String keyPassword);

        boolean TLSConfiguration_addAllowedCertificate(Pointer self, byte[] certificate, int certLen);

        boolean TLSConfiguration_addAllowedCertificateFromFile(Pointer self, String filename);

        boolean TLSConfiguration_addCACertificate(Pointer self, byte[] certificate, int certLen);

        boolean TLSConfiguration_addCACertificateFromFile(Pointer self, String filename);

        void TLSConfiguration_setRenegotiationTime(Pointer self, int timeInMs);

        void TLSConfiguration_setMinTlsVersion(Pointer self, int version);

        void TLSConfiguration_setMaxTlsVersion(Pointer self, int version);

        boolean TLSConfiguration_addCRL(Pointer self, byte[] crl, int crlLen);

        boolean TLSConfiguration_addCRLFromFile(Pointer self, String filename);

        void TLSConfiguration_destroy(Pointer self);
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

    /**
     * Create a new object
     */
    public TLSConfiguration() {
        self = NativeLibTase2.INSTANCE.TLSConfiguration_create();

        if (self == Pointer.NULL)
            throw new RuntimeException("Cannot create native instance");
    }

    /**
     * Enables the validation of the certificate trust chain (enabled by default)
     *
     * @param value true to enable chain validation, false to disable
     */
    public void setChainValidation(boolean value) {
        NativeLibTase2.INSTANCE.TLSConfiguration_setChainValidation(self, value);
    }

    /**
     * Set if only known certificates are accepted.
     *
     * If set to true only known certificates are accepted. Connections with unknown
     * certificates are rejected even if they are signed by a trusted authority.
     *
     * @param value true to enable setting, false otherwise
     */
    public void setAllowOnlyKnownCertificates(boolean value) {
        NativeLibTase2.INSTANCE.TLSConfiguration_setAllowOnlyKnownCertificates(self, value);
    }

    /**
     * Set own certificate (identity)
     *
     * @param certificate the X509 certificate instance
     *
     * @return true, when the certificate was set, false otherwise (e.g. unknown
     *         certificate format)
     */
    public boolean setOwnCertificate(X509Certificate cert) {
        byte[] rawCert;
        try {
            rawCert = cert.getEncoded();
        } catch (CertificateEncodingException e) {
            e.printStackTrace();
            return false;
        }

        return NativeLibTase2.INSTANCE.TLSConfiguration_setOwnCertificate(self, rawCert, rawCert.length);
    }

    /**
     * Set own certificate (identity) from a certificate file
     *
     * @param filename of the certificate file
     *
     * @return true, when the certificate was set, false otherwise (e.g. unknown
     *         certificate format)
     */
    public boolean setOwnCertificate(String filename) {
        return NativeLibTase2.INSTANCE.TLSConfiguration_setOwnCertificateFromFile(self, filename);
    }

    /**
     * Set the own private key
     * 
     * @param key      the private key to use
     * @param password the password of the key or null if the key is not password
     *                 protected
     * @return true, when the key was set, false otherwise (e.g. unknown key format)
     */
    public boolean setOwnKey(PrivateKey key, String password) {
        byte[] encodedKey = key.getEncoded();

        return NativeLibTase2.INSTANCE.TLSConfiguration_setOwnKey(self, encodedKey, encodedKey.length, password);
    }

    /**
     * Set the own private key
     * 
     * @param key      the private key to use
     * @param password the password of the key or null if the key is not password
     *                 protected
     * @return true, when the key was set, false otherwise (e.g. unknown key format)
     */
    public boolean setOwnKey(byte[] key, String password) {
        return NativeLibTase2.INSTANCE.TLSConfiguration_setOwnKey(self, key, key.length, password);
    }

    /**
     * Set the own private key from a key file
     * 
     * @param filename filename/path of the key file
     * @param password the password of the key or null if the key is not password
     *                 protected
     * @return true, when the key was set, false otherwise (e.g. unknown key format)
     */
    public boolean setOwnKey(String filename, String password) {
        return NativeLibTase2.INSTANCE.TLSConfiguration_setOwnKeyFromFile(self, filename, password);
    }

    /**
     * Add a certificate to the list of allowed peer certificates
     * 
     * @param cert the peer certificate
     * @return true, when the certificate was set, false otherwise (e.g. unknown
     *         certificate format)
     */
    public boolean addAllowedCertificate(X509Certificate cert) {
        byte[] rawCert;
        try {
            rawCert = cert.getEncoded();
        } catch (CertificateEncodingException e) {
            return false;
        }

        return NativeLibTase2.INSTANCE.TLSConfiguration_addAllowedCertificate(self, rawCert, rawCert.length);
    }

    /**
     * Add a certificate to the list of allowed peer certificates
     * 
     * @param filename filename of the certificate file
     * @return true, when the certificate was set, false otherwise (e.g. unknown
     *         certificate format)
     */
    public boolean addAllowedCertificate(String filename) {
        return NativeLibTase2.INSTANCE.TLSConfiguration_addAllowedCertificateFromFile(self, filename);
    }

    /**
     * Add a CA certificate used to validate peer certificates
     * 
     * @param cert the CA certificate
     * @return true, when the certificate was set, false otherwise (e.g. unknown
     *         certificate format)
     */
    public boolean addCACertificate(X509Certificate cert) {
        byte[] rawCert;
        try {
            rawCert = cert.getEncoded();
        } catch (CertificateEncodingException e) {
            return false;
        }

        return NativeLibTase2.INSTANCE.TLSConfiguration_addCACertificate(self, rawCert, rawCert.length);
    }

    /**
     * Add a CA certificate used to validate peer certificates
     * 
     * @param filename the CA certificate
     * @return true, when the certificate was set, false otherwise (e.g. unknown
     *         certificate format)
     */
    public boolean addCACertificate(String filename) {
        return NativeLibTase2.INSTANCE.TLSConfiguration_addCACertificateFromFile(self, filename);
    }

    /**
     * Set the renegotiation timeout.
     *
     * After the timeout elapsed a TLS session renegotiation has to occur.
     *
     * @param timeInMs session renegotiation timeout in milliseconds
     */
    public void setRenogotiationTime(int timeInMs) {
        NativeLibTase2.INSTANCE.TLSConfiguration_setRenegotiationTime(self, timeInMs);
    }

    /**
     * Set highest allowed TLS version to use
     * 
     * @param version highest allowed TLS version
     */
    public void setMinTlsVersion(TLSVersion version) {
        NativeLibTase2.INSTANCE.TLSConfiguration_setMinTlsVersion(self, version.getValue());
    }

    /**
     * Set minimal allowed TLS version to use
     * 
     * @param version lowest allowed TLS version
     */
    public void setMaxTlsVersion(TLSVersion version) {
        NativeLibTase2.INSTANCE.TLSConfiguration_setMaxTlsVersion(self, version.getValue());
    }

    /**
     * Add a CRL (certificate revocation list) from a file
     *
     * @param filename of the CRL file
     *
     * @return true, when the CRL was added, false otherwise (e.g. unknown format)
     */
    public boolean addCRLFromFile(String filename) {
        return NativeLibTase2.INSTANCE.TLSConfiguration_addCRLFromFile(self, filename);
    }

    /**
     * Add a CRL (certificate revocation list)
     * 
     * @param crl the CRL
     * @return true, when the CRL was added, false otherwise (e.g. unknown format)
     */
    public boolean addCRL(X509CRL crl) {
        byte[] rawCRL;
        try {
            rawCRL = crl.getEncoded();
        } catch (CRLException e) {
            return false;
        }

        return NativeLibTase2.INSTANCE.TLSConfiguration_addAllowedCertificate(self, rawCRL, rawCRL.length);
    }

    /**
     * Release all resource allocated by the TLSConfiguration instance
     * 
     * Warning: Do not call dispose when the TLSConfiguration instance is still in
     * use by a Client, Server, or Endpoint instance!
     * 
     * NOTE: Do not use the object after calling dispose!
     */
    public void dispose() {
        if (self != Pointer.NULL) {
            NativeLibTase2.INSTANCE.TLSConfiguration_destroy(self);
            self = Pointer.NULL;
        }
    }

    @Override
    protected void finalize() {
        dispose();
    }
}
