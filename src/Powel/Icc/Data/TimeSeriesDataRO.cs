using System;
using System.Data;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using Oracle.ManagedDataAccess.Client;
//using Oracle.ManagedDataAccess.Types;
using Powel.Icc.Common;
using Powel.Icc.Services.Time;
//using log4net;

namespace Powel.Icc.Data
{
    public class TimeSeriesDataRO
    {
        //------------------------------------------------------------------------
        /// <summary>
        /// Time series info (metadata) class with public access to most members.
        /// </summary>
        public class TimeSeriesInfo
        {
            public int tims_key = 0;
            public string tscode;
            public string name;
            public string descript;
            public int docindex;
            public string esttime;
            public string bastime;
            public string startime;
            public string fintime;
            public string lvdate;
            public string lcdate;
            public string lap_date;
            public string tims_vsc;
            public short datasize;
            private byte steptype; // Controlled by tsin_key
                                   /*
                                   TSIN_KEY   STEPTYPE CODE
                                   ---------- ---------- ----------
                                   -1		  2 Knekkpunkt
                                   23		  7 Minutt_15
                                   101		  4 Time
                                   111		  5 Dag
                                   121		  6 Uke
                                   131		  8 Måned
                                   141		  9 År
                                   */
            public CurveType CurveType;
            private PrimaryValueType valueType = PrimaryValueType.Unknown;
            private UtcTime valueTypeValidFrom;
            public short version;
            public short referenc;
            public string hvhcmode;
            public string filename;
            public int paraflag;
            public int grup_key;
            public int hyd_key;
            public int prd_key;
            public int tspr_key;
            public int tsca_key;
            public int tsqu_key;
            public TimeResolution resolution = TimeResolution.Unconstrained;  // Knekkpunkt som default.
            public int tsor_key;
            public int tser_key;
            public int unme_key;  // Ref Unit enum
            public int rc_key;
            public byte convertusinglocaltime;
            public int use_registration_handling;
            public int audit_level;
            public bool isPrecipitation = false;

            public TimeSeriesInfo()
            {
            }

            public TimeSeriesInfo(string fullname)
            {
                this.FullName = fullname;
            }

            public TimeSeriesInfo(DataRow dr) : this(dr, false)
            {
            }

            /// <summary>
            /// Prints all public properties of a Measure Point, excluded any related agreements, components or terminals.
            /// </summary>
            /// <returns></returns>
            public override string ToString()
            {
                string separator = ";";
                return
                    "tims_key: " + tims_key + separator +
                    "tscode: " + tscode + separator +
                    "name: " + name + separator +
                    // "descript: " + @ + separator +
                    // "docindex: " + @ + separator +
                    // "esttime: " + @ + separator +
                    // "bastime: " + @ + separator +
                    // "startime: " + @ + separator +
                    // "fintime: " + @ + separator +
                    // "lvdate: " + @ + separator +
                    // "lcdate: " + @ + separator +
                    // "lap_date: " + @ + separator +
                    // "tims_vsc: " + @ + separator +
                    // "datasize: " + @ + separator +
                    // "CurveType: " + @ + separator +
                    "valueType: " + valueType + separator +
                    "valueTypeValidFrom: " + valueTypeValidFrom + separator +
                    "version: " + version + separator +
                    // "referenc: " + @ + separator +
                    // "hvhcmode: " + @ + separator +
                    // "filename: " + @ + separator +
                    // "paraflag: " + @ + separator +
                    // "grup_key: " + @ + separator +
                    // "hyd_key: " + @ + separator +
                    // "prd_key: " + @ + separator +
                    // "tspr_key: " + @ + separator +
                    // "tsca_key: " + @ + separator +
                    // "tsqu_key: " + @ + separator +
                    // "resolution: " + @ + separator +
                    // "tsor_key: " + @ + separator +
                    // "tser_key: " + @ + separator +
                    // "unme_key: " + @ + separator +
                    // "rc_key: " + @ + separator +
                    // "convertusinglocaltime: " + @ + separator +
                    // "use_registration_handling: " + @ + separator +
                    // "isPrecipitation: " + @ + separator +
                    ""
                    ;
            }

            public TimeSeriesInfo(DataRow dr, bool backPort92)
            {
                tims_key = (int)dr["TIMS_KEY"];
                tscode = (string)dr["TSCODE"];

                if (dr["NAME"] != DBNull.Value) name = (string)dr["NAME"];
                if (dr["DESCRIPT"] != DBNull.Value) descript = (string)dr["DESCRIPT"];
                if (dr["DOCINDEX"] != DBNull.Value) docindex = (int)dr["DOCINDEX"];
                if (dr["ESTTIME"] != DBNull.Value) esttime = (string)dr["ESTTIME"];
                if (dr["BASTIME"] != DBNull.Value) bastime = (string)dr["BASTIME"];
                if (dr["STARTIME"] != DBNull.Value) startime = (string)dr["STARTIME"];
                if (dr["FINTIME"] != DBNull.Value) fintime = (string)dr["FINTIME"];
                if (dr["LVDATE"] != DBNull.Value) lvdate = (string)dr["LVDATE"];
                if (dr["LCDATE"] != DBNull.Value) lcdate = (string)dr["LCDATE"];
                if (dr["LAP_DATE"] != DBNull.Value) lap_date = (string)dr["LAP_DATE"];
                if (dr["TIMS_VSC"] != DBNull.Value) tims_vsc = (string)dr["TIMS_VSC"];
                if (dr["DATASIZE"] != DBNull.Value) datasize = (short)dr["DATASIZE"];
                // steptype = (byte)dr["STEPTYPE"]; // Controlled by tsin_key property: Resolution
                if (dr["CURVETYP"] != DBNull.Value) CurveType = (CurveType)dr["CURVETYP"];
                if (dr["VERSION"] != DBNull.Value) version = (short)dr["VERSION"];
                if (dr["REFERENC"] != DBNull.Value) referenc = (short)dr["REFERENC"];
                if (dr["HVHCMODE"] != DBNull.Value) hvhcmode = (string)dr["HVHCMODE"];
                if (dr["FILENAME"] != DBNull.Value) filename = (string)dr["FILENAME"];
                if (dr["PARAFLAG"] != DBNull.Value) paraflag = (int)dr["PARAFLAG"];
                if (dr["GRUP_KEY"] != DBNull.Value) grup_key = (int)dr["GRUP_KEY"];
                if (dr["HYD_KEY"] != DBNull.Value) hyd_key = (int)dr["HYD_KEY"];
                if (dr["PRD_KEY"] != DBNull.Value) prd_key = (int)dr["PRD_KEY"];
                if (dr["TSPR_KEY"] != DBNull.Value) tspr_key = (int)dr["TSPR_KEY"];
                if (dr["TSCA_KEY"] != DBNull.Value) tsca_key = (int)dr["TSCA_KEY"];
                if (dr["TSQU_KEY"] != DBNull.Value) tsqu_key = (int)dr["TSQU_KEY"];
                if (dr["TSIN_KEY"] != DBNull.Value) Resolution = new TimeResolution((int)dr["TSIN_KEY"]);   // Ref Resolution enum
                if (dr["TSOR_KEY"] != DBNull.Value) tsor_key = (int)dr["TSOR_KEY"];
                if (dr["TSER_KEY"] != DBNull.Value) tser_key = (int)dr["TSER_KEY"];
                if (dr["UNME_KEY"] != DBNull.Value) unme_key = (int)dr["UNME_KEY"];  // Ref Unit enum
                if (dr["RC_KEY"] != DBNull.Value) rc_key = (int)dr["RC_KEY"];
                if (dr["CONVERTUSINGLOCALTIME"] != DBNull.Value) convertusinglocaltime = Convert.ToByte(dr["CONVERTUSINGLOCALTIME"]);
                if (dr["USE_REGISTRATION_HANDLING"] != DBNull.Value) use_registration_handling = Convert.ToInt32(dr["USE_REGISTRATION_HANDLING"]);
                if (dr["AUDIT_LEVEL"] != DBNull.Value) audit_level = Convert.ToByte(dr["AUDIT_LEVEL"]);
                if (!backPort92)
                {
                    if (dr["VLTP_KEY"] != DBNull.Value) valueType = (PrimaryValueType)dr["VLTP_KEY"];
                    if (dr["VLTP_VALFTIME"] != DBNull.Value)
                    {
#if TSS_Availability
                        var cal = RegionalCalendar.CurrentNT;
#else
                        var cal = IccConfiguration.Time.DatabaseCalendar;
#endif
                        valueTypeValidFrom = cal.ToUtcTime(Convert.ToDateTime(dr["VLTP_VALFTIME"]));
                    }
                }
            }

            public TimeResolution Resolution
            {
                get => resolution;
                set
                {
                    resolution = value;
                    steptype = value.Steptype;
                }
            }

            public string TimeStepAttribute
            {
                get
                {
                    if (Resolution == TimeResolution.Unconstrained)
                        return "{VARINT,0}";
                    if (Resolution == TimeResolution.Minute15)
                        return "{MIN15,1}";
                    if (Resolution == TimeResolution.Day)
                        return "{DAY,1}";
                    if (Resolution == TimeResolution.Week)
                        return "{WEEK,1}";
                    if (Resolution == TimeResolution.Month)
                        return "{MONTH,1}";
                    if (Resolution == TimeResolution.Year)
                        return "{YEAR,1}";

                    return "{HOUR,1}";
                }
            }

            public byte Steptype => steptype;
            public int Unit
            {
                get => unme_key;
                set => unme_key = value;
            }

            public string FullName
            {
                get
                {
                    // Make sure filename contains at least a slash
                    if (String.IsNullOrEmpty(filename))
                        filename = "/";

                    // Make sure filename starts with a slash
                    if (!filename.StartsWith(("/")))
                        filename = "/" + filename;

                    // Concatenate filename and tscode
                    var fullName = filename + "/" + tscode;

                    // Remove any double slashes
                    while (fullName.Contains("//"))
                        fullName = fullName.Replace("//", "/");

                    return fullName;
                }
                set
                {
                    // Make sure fullname starts with a slash
                    if (!value.StartsWith("/"))
                        value = "/" + value;

                    // Remove any double slashes
                    while (value.Contains("//"))
                        value = value.Replace("//", "/");

                    // Split fullname into filename and tscode
                    var i = value.LastIndexOf('/');
                    filename = value.Substring(0, i);
                    tscode = value.Substring(i + 1, value.Length - i - 1);

                    // Make sure filename is at least a slash
                    if (filename == "")
                        filename = "/";
                }
            }

            public PrimaryValueType ValueType
            {
                get => valueType;
                set => valueType = value;
            }
            public UtcTime ValueTypeValidFrom
            {
                get => valueTypeValidFrom;
                set => valueTypeValidFrom = value;
            }
        }

    }
}
