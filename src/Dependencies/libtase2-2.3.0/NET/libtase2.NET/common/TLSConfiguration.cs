using System;
using System.Text;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Collections;
using System.Security.Cryptography.X509Certificates;
using System.Security.Cryptography;

namespace TASE2.Library
{
    namespace Common
    {

        [StructLayout(LayoutKind.Sequential)]
        public struct Statistics
        {
            public UInt64 messagesSent;
            public UInt64 messagesRcvd;
            public UInt64 errors;
        }

        /// <summary>
        /// A container for TLS configuration and X.509 certificates.
        /// </summary>
        public class TLSConfiguration : IDisposable
        {
            private IntPtr self = IntPtr.Zero;

            internal IntPtr Self
            {
                get
                {
                    return self;
                }
            }

            private bool allowOnlyKnownCerts = false;
            private bool chainValidation = true;

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            static extern IntPtr TLSConfiguration_create();

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            static extern void TLSConfiguration_destroy(IntPtr self);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            static extern void TLSConfiguration_setAllowOnlyKnownCertificates(IntPtr self, [MarshalAs(UnmanagedType.I1)] bool value);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            static extern void TLSConfiguration_setChainValidation(IntPtr self, [MarshalAs(UnmanagedType.I1)] bool value);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            static extern void TLSConfiguration_setClientMode(IntPtr self);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            [return: MarshalAs(UnmanagedType.I1)]
            static extern bool TLSConfiguration_setOwnCertificate(IntPtr self, byte[] certificate, int certLen);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            [return: MarshalAs(UnmanagedType.I1)]
            static extern bool TLSConfiguration_setOwnCertificateFromFile(IntPtr self, string filename);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            [return: MarshalAs(UnmanagedType.I1)]
            static extern bool TLSConfiguration_setOwnKey(IntPtr self, byte[] key, int keyLen, string keyPassword);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            [return: MarshalAs(UnmanagedType.I1)]
            static extern bool TLSConfiguration_setOwnKeyFromFile(IntPtr self, string filename, string keyPassword);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            [return: MarshalAs(UnmanagedType.I1)]
            static extern bool TLSConfiguration_addAllowedCertificate(IntPtr self, byte[] certificate, int certLen);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            [return: MarshalAs(UnmanagedType.I1)]
            static extern bool TLSConfiguration_addAllowedCertificateFromFile(IntPtr self, string filename);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            [return: MarshalAs(UnmanagedType.I1)]
            static extern bool TLSConfiguration_addCACertificate(IntPtr self, byte[] certificate, int certLen);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            [return: MarshalAs(UnmanagedType.I1)]
            static extern bool TLSConfiguration_addCACertificateFromFile(IntPtr self, string filename);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            [return: MarshalAs(UnmanagedType.I1)]
            static extern bool TLSConfiguration_addCRL(IntPtr self, byte[] crl, int crlLen);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            [return: MarshalAs(UnmanagedType.I1)]
            static extern bool TLSConfiguration_addCRLFromFile(IntPtr self, string filename);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            static extern void TLSConfiguration_setMinTlsVersion(IntPtr self, int version);

            [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
            static extern void TLSConfiguration_setMaxTlsVersion(IntPtr self, int version);

            public TLSConfiguration()
            {
                self = TLSConfiguration_create();
            }

            ~TLSConfiguration()
            {
                Dispose();
            }

            internal IntPtr GetNativeInstance()
            {
                return self;
            }

            /// <summary>
            /// Gets or sets a value indicating whether this <see cref="TASE2.Library.Common.TLSConfiguration"/> allow only peers with known certificates to connect.
            /// </summary>
            /// <value><c>true</c> to allow only known certificates; otherwise, <c>false</c>.</value>
            public bool AllowOnlyKnownCertificates
            {
                set
                {
                    TLSConfiguration_setAllowOnlyKnownCertificates(self, value);
                    allowOnlyKnownCerts = value;
                }
                get
                {
                    return allowOnlyKnownCerts;
                }
            }

            /// <summary>
            /// Enable or disable chain validation.
            /// </summary>
            /// If chain validation is enabled the provided peer certificate will be validated to
            /// be signed with one of the configured CA certificates. If not signed by a known CA
            /// certificate the connection will be rejected.
            /// <value><c>true</c> if chain validation is enabled; otherwise, <c>false</c>.</value>
            public bool ChainValidation
            {
                set
                {
                    TLSConfiguration_setChainValidation(self, value);
                    chainValidation = value;
                }
                get
                {
                    return chainValidation;
                }
            }

            /// <summary>
            /// Configure TLS client mode (usually set by the communication library)
            /// </summary>
            public void SetClientMode()
            {
                TLSConfiguration_setClientMode(self);
            }

            /// <summary>
            /// Sets the own certificate (public key) from a X509 certificate file.
            /// </summary>
            /// The certificate file can contain a base64 representation of the DER encoded certificate
            /// <param name="filename">the name of the certificate file</param>
            public void SetOwnCertificate(string filename)
            {
                if (TLSConfiguration_setOwnCertificateFromFile(self, filename) == false)
                {
                    throw new CryptographicException("Failed to read certificate from file");
                }
            }

            /// <summary>
            /// Sets the own certificate (public key) from a X509 certificate file.
            /// </summary>
            /// The certificate file can contain a base64 representation of the DER encoded certificate
            /// <param name="filename">the name of the certificate file</param>
            public void SetOwnCertificate(X509Certificate2 cert)
            {
                byte[] certBytes = cert.GetRawCertData();

                if (TLSConfiguration_setOwnCertificate(self, certBytes, certBytes.Length) == false)
                {
                    throw new CryptographicException("Failed to set certificate");
                }
            }

            /// <summary>
            /// Adds an allowed peer certificate from a X509 certificate file.
            /// </summary>
            /// The certificate file can contain a base64 representation of the DER encoded certificate
            /// <param name="filename">the name of the certificate file</param>
            public void AddAllowedCertificate(string filename)
            {
                if (TLSConfiguration_addAllowedCertificateFromFile(self, filename) == false)
                {
                    throw new CryptographicException("Failed to read allowed certificate from file");
                }
            }

            /// <summary>
            /// Adds an allowed peer certificate from a X509 certificate structure
            /// </summary>
            /// Multiple CA certificates can be loaded to support multiple CAs
            /// <param name="cert">the CA certificate</param>
            public void AddAllowedCertificate(X509Certificate2 cert)
            {
                byte[] certBytes = cert.GetRawCertData();

                if (TLSConfiguration_addAllowedCertificate(self, certBytes, certBytes.Length) == false)
                {
                    throw new CryptographicException("Failed to add allowed certificate");
                }
            }

            /// <summary>
            /// Adds a CA certificate from a X509 certificate file.
            /// </summary>
            /// The certificate file can contain a base64 representation of the DER encoded certificate
            /// <param name="filename">the name of the certificate file</param>
            public void AddCACertificate(string filename)
            {
                if (TLSConfiguration_addCACertificateFromFile(self, filename) == false)
                {
                    throw new CryptographicException("Failed to read CA certificate from file");
                }
            }

            /// <summary>
            /// Adds a CA certificate from a X509 certificate structure
            /// </summary>
            /// Multiple CA certificates can be loaded to support multiple CAs
            /// <param name="cert">the CA certificate</param>
            public void AddCACertificate(X509Certificate2 cert)
            {
                byte[] certBytes = cert.GetRawCertData();

                if (TLSConfiguration_addCACertificate(self, certBytes, certBytes.Length) == false)
                {
                    throw new CryptographicException("Failed to add CA certificate");
                }
            }

            /// <summary>
            /// Sets the own private key from a key file.
            /// </summary>
            /// The key file can contain a base64 representation of the DER encoded private key
            /// <param name="filename">the name of the key file</param>
            /// <param name="password">the optional password in case the key in the key file is password protected.</param>
            public void SetOwnKey(string filename, string password)
            {
                if (TLSConfiguration_setOwnKeyFromFile(self, filename, password) == false)
                {
                    throw new CryptographicException("Failed to read own key from file");
                }
            }

            /// <summary>
            /// Sets the own private key from a X509 certificate structure
            /// </summary>
            /// <param name="key">The \ref X509Certificate2 object that contains the private key</param>
            /// <param name="password">the optional password in case the key is password protected.</param>
            public void SetOwnKey(X509Certificate2 key, string password)
            {
                byte[] certBytes = key.Export(X509ContentType.Pkcs12);

                if (TLSConfiguration_setOwnKey(self, certBytes, certBytes.Length, password) == false)
                {
                    throw new CryptographicException("Failed to set own key");
                }
            }

            /// <summary>
            /// Add a CRL from a X509 CRL file
            /// </summary>
            /// <param name="filename">the name of the CRL file</param>
            public void AddCRL(string filename)
            {
                if (TLSConfiguration_addCRLFromFile(self, filename) == false)
                {
                    throw new CryptographicException("Failed to read CRL from file");
                }
            }

            /// <summary>
            /// Set minimal allowed TLS version to use
            /// </summary>
            /// <param name="version">lowest allowed TLS version</param>
            public void SetMinTlsVersion(TLSVersion version)
            {
                TLSConfiguration_setMinTlsVersion(self, (int)version);
            }

            /// <summary>
            /// Set highest allowed TLS version to use
            /// </summary>
            /// <param name="version">highest allowed TLS version</param>
            public void SetMaxTlsVersion(TLSVersion version)
            {
                TLSConfiguration_setMaxTlsVersion(self, (int)version);
            }

            /// <summary>
            /// Releases all resource used by the <see cref="TASE2.Library.Common.TLSConfiguration"/> object.
            /// </summary>
            /// <remarks>
            /// Don't call <see cref="Dispose"/> when this instance is still in use by an <see cref="TASE2.Library.Common.Endpoint"/>,
            /// a <see cref="TASE2.Library.client.Client"/>, or <see cref="TASE2.Library.server.Server"/> instance!
            /// Call <see cref="Dispose"/> when you are finished using the <see cref="TASE2.Library.Common.TLSConfiguration"/>.
            /// The <see cref="Dispose"/> method leaves the <see cref="TASE2.Library.Common.TLSConfiguration"/> in an unusable
            /// state. After calling <see cref="Dispose"/>, you must release all references to the
            /// <see cref="TASE2.Library.Common.TLSConfiguration"/> so the garbage collector can reclaim the memory that the
            /// <see cref="TASE2.Library.Common.TLSConfiguration"/> was occupying.</remarks>
            public void Dispose()
            {
                lock (this)
                {
                    if (self != IntPtr.Zero)
                    {
                        TLSConfiguration_destroy(self);
                        self = IntPtr.Zero;
                    }
                }
            }

        }

    }
}

