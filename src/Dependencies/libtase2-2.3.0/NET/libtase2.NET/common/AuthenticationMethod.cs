/*
 *  Copyright 2020 MZ Automation GmbH
 *
 *  This file is part of libtase2
 */

using System;

namespace TASE2.Library.Common
{
    public enum AuthenticationMethod
    {
        /// <summary>
        /// Neither ACSE nor TLS authentication used
        /// </summary>
        NONE = 0,

        /// <summary>
        /// Use ACSE password for peer authentication
        /// </summary>
        PASSWORD = 1,

        /// <summary>
        /// Use ACSE certificate for peer authentication
        /// </summary>
        CERTIFICATE = 2,

        /// <summary>
        /// Use TLS certificate for peer authentication 
        /// </summary>
        TLS = 3,

        /// <summary>
        /// Unknown mechanism for peer authentication
        /// </summary>
        UNKNOWN = 99
    }
}
