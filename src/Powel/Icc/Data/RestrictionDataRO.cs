using System;
using System.Data;
using System.Text;
using System.Collections;
using Oracle.ManagedDataAccess.Client;
//using Powel.Icc.Common;
//using System.Collections.Generic;
using System.Diagnostics;
#if !TSS_Availability
using log4net;
#endif
using Oracle.ManagedDataAccess.Types;
using System.Reflection;
using Powel.Icc.Common;
using System.Collections.Generic;

namespace Powel.Icc.Data
{
    public class RestrictionDataRO
    {
#if TSS_Availability
        private static readonly ConsoleLogger Log = new ConsoleLogger(typeof(Util));
#else
        protected static readonly ILog Log = LogManager.GetLogger(typeof(RestrictionDataRO));
#endif
        //TODO. SIV. So far I did not manage to initiate log4net while running C++ unit tests.
        //Failover solution is to use Console output while running C++ unit test (TimeSeriesDBTests::TestCreateSolverExpressionString)

        private static readonly string UsePminAsDefault = Environment.GetEnvironmentVariable("ICC_RESTR_USE_PMIN_AS_DEFAULT");
        public static bool UseCompAttributesFromDbProc
        {
            get;
            set;
        }


        public static int DefaultGenPriority { get; set; } = -1;
        public static int DefaultGatePriority { get; set; } = -1;
        private static CompAttributesCollection _compAttributes;
        public static CompAttributesCollection CompAttributes
        {
            get
            {
                if (_compAttributes == null)
                {
                    _compAttributes = new CompAttributesCollection();
                }

                return _compAttributes;

            }
            private set => _compAttributes = value;
        }


        public class StructureComponent
        {
            public int HydKey; // RESTR_COMP_CON.Hyd_Key
            public int TimsKey; // RESTR_COMP_CON.Tims_Key
        }

        public class ComponentAttr
        {
            public int AttrId; // COMPATTR.Attr_Id 
            public int HydKey; // COMPATTR.Hyd_Key = HYREL_AC.Hyd_KeyH
            public SimCompType CompType; // HYDCOMP.HydT_Key
            public double Rval; // COMPATTR.Rval
            public int Ival; // COMPATTR.Ival
        }

        public class ComponentRevision
        {
            public DateTime PeriodStart;
            public DateTime PeriodEnd;
            public decimal Value; // Why not double?
            public short FunctionType;
            public short FrequencyCode;
            public short Hours;
        }

        public class ComponentRestriction
        {
            public short Priority;
            public int TimsKey;
            public int CompKey;
        }

        /// <summary>
        /// The class to represent restriction attributes. To be merged with 
        /// </summary>
        public class ComponentInfo
        {
            public string What;
            public int Key; // RESTR_COMP_CON.Comp_Key/Tims_Key dependent on value of What
            public int comp_key; // RESTR_COMP_CON.Comp_Key
            public int tims_key; // RESTR_COMP_CON.Tims_Key
            public double defval;
            public double defval_orig;
            public int oprt_key; // RESTR_COMP_CON.OpRt_Key
            public int hyd_key; // RESTR_COMP_CON.Hyd_Key
            public int hydp_key; // RESTR_COMP_CON.Hydp_Key
            public SimCompType CompType; // HYDCOMP.HydT_Key
            public CurveType CurveType;
            public StructureComponent[] StructureComponents;
            public ComponentAttr[] Attributes;
            public double NumAttrsTotalSum;
            public double PumpMaxFlow;
            public int[] TimsKeysNotReferredByPump;
            public int NsTimsKey;
            public ComponentRevision[] Revisions;
            public ComponentRestriction[] Restrictions;
            public string VtsExpression;
            public int ErrorNo;
            public string ErrorText;

            public ComponentInfo()
            {
                What = "";
                Key = -1;
                comp_key = -1;
                tims_key = -1;
                defval = double.NaN;
                defval_orig = double.NaN;
                oprt_key = -1;
                hyd_key = -1;
                hydp_key = -1;
                CompType = SimCompType.Plant;
                CurveType = CurveType.Step;
                StructureComponents = null;
                Attributes = null;
                NumAttrsTotalSum = double.NaN;
                TimsKeysNotReferredByPump = null;
                NsTimsKey = -1;
                Revisions = null;
                Restrictions = null;
                VtsExpression = string.Empty;
                ErrorNo = 0;
                ErrorText = string.Empty;
            }

            public ComponentInfo(string what, int key) : this()
            {
                What = string.IsNullOrEmpty(what) ? "" : what.Trim();
                Key = key;
            }
        }

        /// <summary>
        /// The class to hold info found in RESTR_COMP_CON
        /// </summary>
        public class RestrCompCon
        {
            public int comp_key = 0;
            public int hyd_key;
            public int hydp_key = 0;
            public int oprt_key;
            public int tims_key;
            public int owner = 1; //TODO add to constructor

            public RestrCompCon(int hyd_key, int hydp_key, int oprt_key)
            {
                this.hyd_key = hyd_key;
                this.hydp_key = hydp_key;
                this.oprt_key = oprt_key;

            }

            public static bool operator ==(RestrCompCon a, RestrCompCon b)
            {
                return a.comp_key == b.comp_key && a.hyd_key == b.hyd_key && a.oprt_key == b.oprt_key;
            }

            public static bool operator !=(RestrCompCon a, RestrCompCon b)
            {
                return !(a.comp_key == b.comp_key && a.hyd_key == b.hyd_key && a.oprt_key == b.oprt_key);
            }

            public override bool Equals(Object o)
            {
                if (o is RestrCompCon)
                    return Equals(o as RestrCompCon);
                return false;
            }

            public bool Equals(RestrCompCon c)
            {
                return this.comp_key == c.comp_key && this.hyd_key == c.hyd_key && this.oprt_key == c.oprt_key;
            }

            public override int GetHashCode()
            {
                return GetHashCode();
            }
        }

        /// <summary>
        /// Bulk reads availability data defined by the restriction expression '!RestrictionExpand:{COMP_KEY|TIMS_KEY},{key}'
        /// </summary>
        /// <param name="compInfos">Contains key info. What = {COMP_KEY|TIMS_KEY}, Key = numeric key value. 
        /// Each pair refers to a group of rows in DB table RESTR_COMP_COM identified by the numeric key</param>
        /// <returns>Number of elements in compInfos successfully filled out with data from DB</returns>
        public static int BulkReadAvailability(ref ComponentInfo[] compInfos
            , out OracleRefCursor rcCompAttributes)
        {
            Profiler.Log(5000 + 460);
            rcCompAttributes = null;
            var compRowCount = 0;
            var revRowCount = 0;
            var compStructureRowCount = 0;
            var attributeRowCount = 0;
            var restrRowCount = 0;
            var openConnElapsed = 0L;
            var bulkReadElapsed = 0L;
            var fetchComponentsElapsed = 0L;
            var fetchStructureComponentsElapsed = 0L;
            var fetchAttributesElapsed = 0L;
            var fetchRevisionsElapsed = 0L;
            var fetchRestrictionsElapsed = 0L;

            var compCount = compInfos.Length;
            Log.Warn($"Enter\n\tcompCount = {compCount}");

            if (compCount == 0)
            {
                Log.Warn($"Exit\n\tcompCount = {compCount}");
                return compRowCount;
            }

            var keyTypes = new string[compCount];
            var keys = new int[compCount];
            var notReadCompIndexes = new int[compCount];
            var compIndex = 0;
            for (var i = 0; i < compCount; i++)
            {
                var compInfo = compInfos[i];
                Log.Debug($"\t{compIndex}. compInfo.What = {compInfo.What}, Key = {compInfo.Key}");
                if (compInfo.What != "COMP_KEY" && compInfo.What != "TIMS_KEY")
                {
                    continue;
                }
                keyTypes[compIndex] = compInfo.What;
                keys[compIndex] = compInfo.Key;
                notReadCompIndexes[compIndex] = i;
                compIndex++;
            }

            var notReadCompCount = compIndex;

            if (notReadCompCount == 0)
            {
                Log.Warn($"Exit\n\tnotReadCompCount = {notReadCompCount}");
                return compRowCount;
            }

            Profiler.Log(5000 + 470);

            if (notReadCompCount < compCount)
            {
                Array.Resize(ref keyTypes, notReadCompCount);
                Array.Resize(ref keys, notReadCompCount);
            }

            using (var cmd = new OracleCommand())
            {
                Profiler.Log(5000 + 480);
                cmd.CommandType = CommandType.StoredProcedure;
                cmd.CommandText = "ICC_Sim.BulkReadAvailability" +
                                  (UseCompAttributesFromDbProc ? "Ex" : "");

                Profiler.Log(5000 + 481, "cmd.Parameters.Add(Keys");
                var parKeys = cmd.Parameters.Add("Keys", OracleDbType.Decimal);
                parKeys.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
                parKeys.Size = notReadCompCount;
                parKeys.Direction = ParameterDirection.Input;
                parKeys.Value = keys;

                Profiler.Log(5000 + 487);
                var parKeyTypes = cmd.Parameters.Add("KeyTypes", OracleDbType.Varchar2);
                parKeyTypes.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
                parKeyTypes.Size = notReadCompCount;
                parKeyTypes.Direction = ParameterDirection.Input;
                parKeyTypes.Value = keyTypes;

                var parRcComponents = cmd.Parameters.Add("rcComponents", OracleDbType.RefCursor);
                parRcComponents.Direction = ParameterDirection.Output;

                var parRcStructureComponents = cmd.Parameters.Add("rcStructureComponents", OracleDbType.RefCursor);
                parRcStructureComponents.Direction = ParameterDirection.Output;

                var parRcAttributes = cmd.Parameters.Add("rcAttributes", OracleDbType.RefCursor);
                parRcAttributes.Direction = ParameterDirection.Output;

                var parRcRevisions = cmd.Parameters.Add("rcRevisions", OracleDbType.RefCursor);
                parRcRevisions.Direction = ParameterDirection.Output;

                var parRcRestrictions = cmd.Parameters.Add("rcDynamicRestrictions", OracleDbType.RefCursor);
                parRcRestrictions.Direction = ParameterDirection.Output;

                OracleParameter parRcCompAttributes = null;
                if (UseCompAttributesFromDbProc)
                {
                    parRcCompAttributes = cmd.Parameters.Add("rcCompAttributes", OracleDbType.RefCursor);
                    parRcCompAttributes.Direction = ParameterDirection.Output;
                }

                Profiler.Log(5000 + 488);
                using (var conn = new OracleConnection())
                {
                    Profiler.Log(5000 + 490, "Calling PrepareConnection");
                    var stopwatch = Stopwatch.StartNew();
                    Util.PrepareConnection(conn);
                    Log.Warn($"\tcmd.Connection.State = {conn.State}\n\tConnectionString:\n\t{conn.ConnectionString}");
                    stopwatch.Stop();
                    openConnElapsed = stopwatch.ElapsedMilliseconds;
                    stopwatch.Reset(); stopwatch.Start();
                    Profiler.Log(5000 + 500);
                    Util.ExecuteCommand(cmd, conn);
                    stopwatch.Stop();
                    bulkReadElapsed = stopwatch.ElapsedMilliseconds;

                    stopwatch.Reset(); stopwatch.Start();
                    Profiler.Log(5000 + 510, "BulkReadAvailability. Reading REFCURSURs");
                    var rcComponents = (OracleRefCursor)parRcComponents.Value;
                    compRowCount = BulkFetchComponents(compInfos, rcComponents, notReadCompCount, notReadCompIndexes);
                    stopwatch.Stop();
                    fetchComponentsElapsed = stopwatch.ElapsedMilliseconds;

                    stopwatch.Reset(); stopwatch.Start();
                    //Profiler.Log(5000 + 520);
                    var rcStructureComponents = (OracleRefCursor)parRcStructureComponents.Value;
                    compStructureRowCount = BulkFetchStructureComponents(compInfos, rcStructureComponents, notReadCompCount, notReadCompIndexes);
                    stopwatch.Stop();
                    fetchStructureComponentsElapsed = stopwatch.ElapsedMilliseconds;

                    stopwatch.Reset(); stopwatch.Start();
                    //Profiler.Log(5000 + 530);
                    var rcAttributes = (OracleRefCursor)parRcAttributes.Value;
                    attributeRowCount = BulkFetchAttributes(compInfos, rcAttributes, notReadCompCount, notReadCompIndexes);
                    stopwatch.Stop();
                    fetchAttributesElapsed = stopwatch.ElapsedMilliseconds;

                    stopwatch.Reset(); stopwatch.Start();
                    //Profiler.Log(5000 + 540);
                    var rcRevisions = (OracleRefCursor)parRcRevisions.Value;
                    revRowCount = BulkFetchRevisions(compInfos, rcRevisions, notReadCompCount, notReadCompIndexes);
                    stopwatch.Stop();
                    fetchRevisionsElapsed = stopwatch.ElapsedMilliseconds;

                    stopwatch.Reset(); stopwatch.Start();
                    //Profiler.Log(5000 + 550);
                    var rcRestrictions = (OracleRefCursor)parRcRestrictions.Value;
                    restrRowCount = BulkFetchRestrictions(compInfos, rcRestrictions, notReadCompCount, notReadCompIndexes);
                    stopwatch.Stop();
                    fetchRestrictionsElapsed = stopwatch.ElapsedMilliseconds;

                    if (UseCompAttributesFromDbProc && _compAttributes == null)
                    {
                        rcCompAttributes = (OracleRefCursor)parRcCompAttributes.Value;
                        CompAttributes = new CompAttributesCollection(rcCompAttributes);
                    }
                }
            }

            Profiler.Log(5000 + 560);
            var totalElapsed =
                openConnElapsed + bulkReadElapsed + fetchComponentsElapsed +
                fetchStructureComponentsElapsed + fetchAttributesElapsed +
                fetchRevisionsElapsed + fetchRestrictionsElapsed +
                //fetchVtsExprElapsed +
                0;

            Log.Warn($"Exit\n\tcompRowCount = {compRowCount}" +
                $"\n\trevRowCount = {revRowCount}" +
                $"\n\tcompStructureRowCount = {compStructureRowCount}" +
                $"\n\tattributeRowCount = {attributeRowCount}" +
                $"\n\trestrRowCount = {restrRowCount}" +
                $"\n\ttotalElapsed = {totalElapsed}" +
                $"\n\topenConnElapsed = {openConnElapsed}, {Math.Round(100.0 * (double)openConnElapsed / totalElapsed)}%" +
                $"\n\tbulkReadElapsed = {bulkReadElapsed}, {Math.Round(100.0 * (double)bulkReadElapsed / totalElapsed)}%" +
                $"\n\tfetchComponentsElapsed = {fetchComponentsElapsed}, {Math.Round(100.0 * (double)fetchComponentsElapsed / totalElapsed)}%" +
                $"\n\tfetchStructureComponentsElapsed = {fetchStructureComponentsElapsed}, {Math.Round(100.0 * (double)fetchStructureComponentsElapsed / totalElapsed)}%" +
                $"\n\tfetchAttributesElapsed = {fetchAttributesElapsed}, {Math.Round(100.0 * (double)fetchAttributesElapsed / totalElapsed)}%" +
                $"\n\tfetchRevisionsElapsed = {fetchRevisionsElapsed}, {Math.Round(100.0 * (double)fetchRevisionsElapsed / totalElapsed)}%" +
                $"\n\tfetchRestrictionsElapsed = {fetchRestrictionsElapsed}, {Math.Round(100.0 * (double)fetchRestrictionsElapsed / totalElapsed)}%" +
                $"{Util.LogSeparator}");

            Profiler.Log(5000 + 570);
            return compRowCount;
        }

        private static int BulkFetchStructureComponents(
            ComponentInfo[] compInfos, OracleRefCursor rcStructureComponents, int notReadCompCount, int[] notReadCompIndexes)
        {
            var compStructureRowCount = 0;
            //rcStructureComponents.FetchSize = 200;
            var rdStructureComponents = rcStructureComponents.GetDataReader();
            var prevKeyOrder = -1;
            var curCompChildCount = -1;
            StructureComponent[] curStructureComponents = null;
            var curCompChildIndex = 0;
            while (rdStructureComponents.Read())
            {
                var objKeyOrder = rdStructureComponents["PARENTKEY_ORDER"];
                bool isKeyOrderValid;
                var keyOrder = GetKeyOrder(objKeyOrder, notReadCompCount, out isKeyOrderValid);
                if (!isKeyOrderValid)
                    continue;

                if (prevKeyOrder != keyOrder)
                {
                    // Find parent component in compInfos
                    var curCompIndex = notReadCompIndexes[keyOrder - 1];
                    Log.Debug($"\n\t[next] keyOrder = {keyOrder}, nextCompIndex = {curCompIndex}" +
                        $"\n\t{{{compInfos[curCompIndex].What},{compInfos[curCompIndex].Key}}}");
                    prevKeyOrder = keyOrder;
                    curCompChildCount = rdStructureComponents["COMPCHILD_COUNT"] != DBNull.Value
                        ? Convert.ToInt32(rdStructureComponents["COMPCHILD_COUNT"])
                        : -1;
                    if (curCompChildCount <= 0)
                    {
                        Log.Error($"\tInternal error: curCompChildCount (COMPCHILD_COUNT column value) = {curCompChildCount} <= 0");
                        continue;
                    }
                    curStructureComponents = new StructureComponent[curCompChildCount];
                    compInfos[curCompIndex].StructureComponents = curStructureComponents;
                    curCompChildIndex = 0;
                }
                if (curStructureComponents == null)
                {
                    //Quite impossible, but just in case:
                    Log.Error($"\n\tInternal error: curStructureComponents == null. keyOrder = {keyOrder}");
                    continue;
                }
                if (curCompChildIndex >= curCompChildCount)
                {
                    //Quite impossible, but just in case:
                    Log.Error($"\n\tInternal [DB proc] error: curCompChildIndex = {curCompChildIndex} >= curCompChildCount = {curCompChildCount}");
                    continue;
                }

                var hydKey = rdStructureComponents["HYD_KEY"] != DBNull.Value ? Convert.ToInt32(rdStructureComponents["HYD_KEY"]) : -1;
                var timsKey = rdStructureComponents["TIMS_KEY"] != DBNull.Value ? Convert.ToInt32(rdStructureComponents["TIMS_KEY"]) : -1;
                //var compType = rdStructureComponents["HYDT_KEY"] != DBNull.Value
                //    ? (SimCompType)Convert.ToInt32(rdStructureComponents["HYDT_KEY"])
                //    : SimCompType.Undefined;

                Log.Debug($"\n\tcurCompChildIndex = {curCompChildIndex}, hydKey = {hydKey}" +
                    $"\n\ttimsKey = {timsKey}");

                curStructureComponents[curCompChildIndex] = new StructureComponent()
                {
                    HydKey = hydKey,
                    TimsKey = timsKey,
                    //CompKey = compKey,
                };

                ++curCompChildIndex;
                ++compStructureRowCount;
            }
            return compStructureRowCount;
        }

        private static int BulkFetchAttributes(
            ComponentInfo[] compInfos, OracleRefCursor rcAttributes, int notReadCompCount, int[] notReadCompIndexes)
        {
            var attrRowCount = 0;
            //rcAttributes.FetchSize = 200;
            var rdAttributes = rcAttributes.GetDataReader();
            var prevKeyOrder = -1;
            var curAttrCount = -1;
            ComponentAttr[] curAttributes = null;
            var curAttrIndex = 0;
            while (rdAttributes.Read())
            {
                var objKeyOrder = rdAttributes["PARENTKEY_ORDER"];
                var keyOrder = GetKeyOrder(objKeyOrder, notReadCompCount, out var isKeyOrderValid);
                if (!isKeyOrderValid)
                    continue;

                var curCompIndex = -1;

                if (prevKeyOrder != keyOrder)
                {
                    // Find parent component in compInfos
                    curCompIndex = notReadCompIndexes[keyOrder - 1];
                    Log.Debug($"\n\t[next] keyOrder = {keyOrder}, nextCompIndex = {curCompIndex}" +
                        $"\n\t{{{compInfos[curCompIndex].What},{compInfos[curCompIndex].Key}}}");
                    prevKeyOrder = keyOrder;
                    curAttrCount = rdAttributes["ATTRIBUTE_COUNT"] != DBNull.Value
                        ? Convert.ToInt32(rdAttributes["ATTRIBUTE_COUNT"])
                        : -1;
                    if (curAttrCount <= 0)
                    {
                        Log.Error($"\tInternal error: curAttrCount (ATTRIBUTE_COUNT column value) = {curAttrCount} <= 0");
                        continue;
                    }
                    curAttributes = new ComponentAttr[curAttrCount];
                    compInfos[curCompIndex].Attributes = curAttributes;
                    curAttrIndex = 0;
                }
                if (curAttributes == null)
                {
                    //Quite impossible, but just in case:
                    Log.Error($"\n\tInternal error: curAttributes == null. keyOrder = {keyOrder}");
                    continue;
                }
                if (curAttrIndex >= curAttrCount)
                {
                    //Quite impossible, but just in case:
                    Log.Error($"\n\tInternal [DB proc] error: curAttrIndex = {curAttrIndex} >= curAttrCount = {curAttrCount}");
                    continue;
                }

                var attrId = rdAttributes["ATTR_ID"] != DBNull.Value ? Convert.ToInt32(rdAttributes["ATTR_ID"]) : -1;
                var hydKey = rdAttributes["HYD_KEY"] != DBNull.Value ? Convert.ToInt32(rdAttributes["HYD_KEY"]) : -1;
                var compType = rdAttributes["HYDT_KEY"] != DBNull.Value
                    ? (SimCompType)Convert.ToInt32(rdAttributes["HYDT_KEY"])
                    : SimCompType.Undefined;
                var rval = rdAttributes["RVAL"] != DBNull.Value ? Convert.ToDouble(rdAttributes["RVAL"]) : double.NaN;
                var ival = rdAttributes["IVAL"] != DBNull.Value ? Convert.ToInt32(rdAttributes["IVAL"]) : int.MaxValue;
                if (curAttrIndex == 0)
                {
                    var maxProduction = rdAttributes["MAXPRODUCTION"] != DBNull.Value ? Convert.ToDouble(rdAttributes["MAXPRODUCTION"]) : double.NaN;
                    var maxFlow = rdAttributes["MAXFLOW"] != DBNull.Value ? Convert.ToDouble(rdAttributes["MAXFLOW"]) : double.NaN;
                    var pumpMaxFlow = rdAttributes["PUMPMAXFLOW"] != DBNull.Value ? Convert.ToDouble(rdAttributes["PUMPMAXFLOW"]) : double.NaN;
                    compInfos[curCompIndex].NumAttrsTotalSum = double.IsNaN(maxProduction) ? maxFlow : maxProduction;
                    compInfos[curCompIndex].PumpMaxFlow = pumpMaxFlow;

                    Log.Debug($"\n\tcurCompIndex = {curCompIndex}, attrId = {attrId}" +
                        $"\n\thydKey = {hydKey}, compType = {compType}" +
                        $"\n\tmaxProduction = {maxProduction}, maxFlow = {maxFlow}, pumpMaxFlow = {pumpMaxFlow}");
                }

                Log.Debug($"\n\tcurAttrIndex = {curAttrIndex}, attrId = {attrId}" +
                    $"\n\thydKey = {hydKey}, compType = {compType}" +
                    $"\n\trval = {rval}, ival = {ival}");

                curAttributes[curAttrIndex] = new ComponentAttr()
                {
                    AttrId = attrId,
                    HydKey = hydKey,
                    CompType = compType,
                    Rval = rval,
                    Ival = ival,
                };

                ++curAttrIndex;
                ++attrRowCount;
            }
            return attrRowCount;
        }

        private static int BulkFetchRestrictions(
            ComponentInfo[] compInfos, OracleRefCursor rcRestrictions, int notReadCompCount, int[] notReadCompIndexes)
        {
            var restrRowCount = 0;
            //rcRestrictions.FetchSize = 200;
            var rdRestrictions = rcRestrictions.GetDataReader();
            var prevKeyOrder = -1;
            var curCompRestrCount = -1;
            ComponentRestriction[] curCompRestrictions = null;
            var curCompRestrIndex = 0;
            while (rdRestrictions.Read())
            {
                var objKeyOrder = rdRestrictions["PARENTKEY_ORDER"];
                bool isKeyOrderValid;
                var keyOrder = GetKeyOrder(objKeyOrder, notReadCompCount, out isKeyOrderValid);
                if (!isKeyOrderValid)
                    continue;

                if (prevKeyOrder != keyOrder)
                {
                    // Find parent component in compInfos
                    var curCompIndex = notReadCompIndexes[keyOrder - 1];
                    Log.Debug($"\n\t[next] keyOrder = {keyOrder}, nextCompIndex = {curCompIndex}" +
                        $"\n\t{{{compInfos[curCompIndex].What},{compInfos[curCompIndex].Key}}}");
                    prevKeyOrder = keyOrder;
                    curCompRestrCount = rdRestrictions["COMPRESTRICTION_COUNT"] != DBNull.Value
                        ? Convert.ToInt32(rdRestrictions["COMPRESTRICTION_COUNT"])
                        : -1;
                    if (curCompRestrCount <= 0)
                    {
                        Log.Error(
                            $"\n\tInternal error: curCompRestrCount (COMPRESTRICTION_COUNT column value) = {curCompRestrCount} <= 0");
                        continue;
                    }
                    curCompRestrictions = new ComponentRestriction[curCompRestrCount];
                    compInfos[curCompIndex].Restrictions = curCompRestrictions;
                    curCompRestrIndex = 0;
                }
                if (curCompRestrictions == null)
                {
                    //Quite impossible, but just in case:
                    Log.Error($"\n\tInternal error: curCompRestrictions == null. keyOrder = {keyOrder}");
                    continue;
                }
                if (curCompRestrIndex >= curCompRestrCount)
                {
                    //Quite impossible, but just in case:
                    Log.Error(
                        $"\tInternal [DB proc] error: curCompRestrIndex = {curCompRestrIndex} >= curCompRestrCount = {curCompRestrCount}");
                    continue;
                }

                var priority = rdRestrictions["PRIORITY"] != DBNull.Value
                    ? Convert.ToInt16(rdRestrictions["PRIORITY"])
                    : (short)1;
                var timsKey = rdRestrictions["TIMS_KEY"] != DBNull.Value ? Convert.ToInt32(rdRestrictions["TIMS_KEY"]) : -1;
                var compKey = rdRestrictions["COMP_KEY"] != DBNull.Value ? Convert.ToInt32(rdRestrictions["COMP_KEY"]) : -1;

                Log.Debug($"\n\tcurCompRestrIndex = {curCompRestrIndex}, priority = {priority}" +
                    $"\n\ttimsKey = {timsKey}, compKey = {compKey}");

                curCompRestrictions[curCompRestrIndex] = new ComponentRestriction()
                {
                    Priority = priority,
                    TimsKey = timsKey,
                    CompKey = compKey,
                };

                ++curCompRestrIndex;
                ++restrRowCount;
            }
            return restrRowCount;
        }

        private static int BulkFetchRevisions(
            ComponentInfo[] compInfos, OracleRefCursor rcRevisions, int notReadCompCount, int[] notReadCompIndexes)
        {
            var revRowCount = 0;
            //rcRevisions.FetchSize = 200;
            var rdRevisions = rcRevisions.GetDataReader();
            var prevKeyOrder = -1;
            var curCompRevCount = -1;
            ComponentRevision[] curCompRevisions = null;
            var curCompRevIndex = 0;
            while (rdRevisions.Read())
            {
                var objKeyOrder = rdRevisions["PARENTKEY_ORDER"];
                bool isKeyOrderValid;
                var keyOrder = GetKeyOrder(objKeyOrder, notReadCompCount, out isKeyOrderValid);
                if (!isKeyOrderValid)
                    continue;

                if (prevKeyOrder != keyOrder)
                {
                    // Find parent component in compInfos
                    var curCompIndex = notReadCompIndexes[keyOrder - 1];
                    Log.Debug($"\n\t[next] keyOrder = {keyOrder}, nextCompIndex = {curCompIndex}" +
                        $"\n\t{{{compInfos[curCompIndex].What},{compInfos[curCompIndex].Key}}}");
                    prevKeyOrder = keyOrder;
                    curCompRevCount = rdRevisions["COMPREVISION_COUNT"] != DBNull.Value
                        ? Convert.ToInt32(rdRevisions["COMPREVISION_COUNT"])
                        : -1;
                    if (curCompRevCount <= 0)
                    {
                        Log.Error($"\n\tInternal error: curCompRevCount (COMPREVISION_COUNT column value) = {curCompRevCount} <= 0");
                        continue;
                    }
                    curCompRevisions = new ComponentRevision[curCompRevCount];
                    compInfos[curCompIndex].Revisions = curCompRevisions;
                    curCompRevIndex = 0;
                }
                if (curCompRevisions == null)
                {
                    //Quite impossible, but just in case:
                    Log.Error($"\n\tInternal error: curCompRevisions == null. keyOrder = {keyOrder}");
                    continue;
                }
                if (curCompRevIndex >= curCompRevCount)
                {
                    //Quite impossible, but just in case:
                    Log.Error($"\tInternal [DB proc] error: curCompRevIndex = {curCompRevIndex} >= curCompRevCount = {curCompRevCount}");
                    continue;
                }


                var periodStart = rdRevisions["PERIODSTART"] != DBNull.Value
                    ? Convert.ToDateTime(rdRevisions["PERIODSTART"])
                    : default(DateTime);
                var periodEnd = rdRevisions["PERIODEND"] != DBNull.Value
                    ? Convert.ToDateTime(rdRevisions["PERIODEND"])
                    : default(DateTime);
                var functionType = rdRevisions["FUNCTIONTYPE"] != DBNull.Value
                    ? Convert.ToInt16(rdRevisions["FUNCTIONTYPE"])
                    : (short)-1;
                var value = rdRevisions["VALUE"] != DBNull.Value ? Convert.ToDecimal(rdRevisions["VALUE"]) : Decimal.MinValue;
                var frequencyCode = rdRevisions["FREQUENCYCODE"] != DBNull.Value
                    ? Convert.ToInt16(rdRevisions["FREQUENCYCODE"])
                    : (short)0;
                var hours = rdRevisions["HOURS"] != DBNull.Value ? Convert.ToInt16(rdRevisions["HOURS"]) : (short)0;

                Log.Debug($"\n\tcurCompRevIndex = {curCompRevIndex}, periodStart = {periodStart}, periodEnd = {periodEnd}" +
                    $"\n\tfunctionType = {functionType}, value = {value}" +
                    $"\n\tfrequencyCode = {frequencyCode}, hours = {hours}");

                curCompRevisions[curCompRevIndex] = new ComponentRevision()
                {
                    PeriodStart = periodStart,
                    PeriodEnd = periodEnd,
                    FunctionType = functionType,
                    Value = value,
                    FrequencyCode = frequencyCode,
                    Hours = hours,
                };

                ++curCompRevIndex;
                ++revRowCount;
            }
            return revRowCount;
        }

        private static int BulkFetchComponents(
            ComponentInfo[] compInfos, OracleRefCursor rcComponents, int notReadCompCount, int[] notReadCompIndexes)
        {
            var compRowCount = 0;
            rcComponents.FetchSize = 200;
            var rdComponents = rcComponents.GetDataReader();
            while (rdComponents.Read())
            {
                var objKeyOrder = rdComponents["KEY_ORDER"];
                var keyOrder = GetKeyOrder(objKeyOrder, notReadCompCount, out var isKeyOrderValid);
                if (!isKeyOrderValid)
                    continue;

                Log.Debug($"\n\tkeyOrder = {keyOrder}");
                var key = rdComponents["KEY"] != DBNull.Value ? Convert.ToInt32(rdComponents["KEY"]) : -1;
                var what = rdComponents["KEY_TYPE"] != DBNull.Value ? rdComponents["KEY_TYPE"].ToString() : "missing";
                var notReadCompIndex = notReadCompIndexes[keyOrder - 1];
                if (compInfos[notReadCompIndex].Key != key || compInfos[notReadCompIndex].What != what)
                {
                    Log.Error($"\tInternal error: returned pair with keyOrder = {keyOrder} {{{what},{key}}}" +
                        $" does not match input array element with notReadCompIndex = {notReadCompIndex}" +
                        $" {{{compInfos[notReadCompIndex].What},{compInfos[notReadCompIndex].Key}}}");
                    continue;
                }

                var compKey = rdComponents["COMP_KEY"] != DBNull.Value ? Convert.ToInt32(rdComponents["COMP_KEY"]) : -1;
                var hydKey = rdComponents["HYD_KEY"] != DBNull.Value ? Convert.ToInt32(rdComponents["HYD_KEY"]) : -1;
                var compType = rdComponents["HYDT_KEY"] != DBNull.Value
                    ? (SimCompType)Convert.ToInt32(rdComponents["HYDT_KEY"])
                    : SimCompType.Undefined;
                var opTypeKey = rdComponents["OPRT_KEY"] != DBNull.Value ? Convert.ToInt32(rdComponents["OPRT_KEY"]) : -1;
                var timsKey = rdComponents["TIMS_KEY"] != DBNull.Value ? Convert.ToInt32(rdComponents["TIMS_KEY"]) : -1;
                var curveType = rdComponents["CURVETYPE"] != DBNull.Value
                    ? (Convert.ToInt32(rdComponents["CURVETYPE"]) == 0 ? CurveType.Linear : CurveType.Step)
                    : CurveType.Step;
                var hydParentKey = rdComponents["HYDP_KEY"] != DBNull.Value ? Convert.ToInt32(rdComponents["HYDP_KEY"]) : -1;
                var defaultLimit = rdComponents["DEFAULTLIMIT"] != DBNull.Value
                    ? Convert.ToDouble(rdComponents["DEFAULTLIMIT"])
                    : double.NaN;
                var nsTimsKey = rdComponents["NETSUBSCR_TIMS_KEY"] != DBNull.Value
                    ? Convert.ToInt32(rdComponents["NETSUBSCR_TIMS_KEY"])
                    : -1;
                var errNo = rdComponents["ERRNO"] != DBNull.Value ? Convert.ToInt32(rdComponents["ERRNO"]) : 0;
                var errText = rdComponents["ERRTEXT"] != DBNull.Value ? rdComponents["ERRTEXT"].ToString() : "";

                Log.Debug($"\n\tnotReadCompIndex = {notReadCompIndex}, compKey = {compKey}" +
                    $", timsKey = {timsKey}, string.IsNullOrEmpty(UsePminAsDefault) = {string.IsNullOrEmpty(UsePminAsDefault)}");

                compInfos[notReadCompIndex].comp_key = compKey;
                compInfos[notReadCompIndex].hyd_key = hydKey;
                compInfos[notReadCompIndex].CompType = compType;
                compInfos[notReadCompIndex].oprt_key = opTypeKey;
                compInfos[notReadCompIndex].tims_key = timsKey;
                compInfos[notReadCompIndex].CurveType = curveType;
                compInfos[notReadCompIndex].hydp_key = hydParentKey;

                if (opTypeKey == (int)CompAttribute.PMin && compType == SimCompType.Plant && string.IsNullOrEmpty(UsePminAsDefault))
                {
                    //SIV. The same logic as in FetchDefaultLimitForComponent:
                    defaultLimit = 0.0;
                }
                compInfos[notReadCompIndex].defval = defaultLimit;
                compInfos[notReadCompIndex].defval_orig = defaultLimit; //copy of defval since defval may be changed later

                compInfos[notReadCompIndex].NsTimsKey = nsTimsKey;
                compInfos[notReadCompIndex].NsTimsKey = nsTimsKey;
                compInfos[notReadCompIndex].ErrorNo = errNo;
                compInfos[notReadCompIndex].ErrorText = errText;

                ++compRowCount;
            }
            return compRowCount;
        }

        private static int GetKeyOrder(object objKeyOrder, int notReadCompCount, out bool isKeyOrderValid)
        {
            isKeyOrderValid = true;
            if (objKeyOrder == DBNull.Value)
            {
                isKeyOrderValid = false;
                Log.Error($"\n\tInternal error: reader[KEY_ORDER] == DBNull.Value");
                return -1;
            }

            var keyOrder = Convert.ToInt32(objKeyOrder);

            if (keyOrder > notReadCompCount)
            {
                isKeyOrderValid = false;
                Log.Error($"\n\tInternal error: keyOrder = {keyOrder} > notReadCompCount = {notReadCompCount}");
            }
            else if (keyOrder <= 0)
            {
                isKeyOrderValid = false;
                Log.Error($"\n\tInternal error: keyOrder = {keyOrder} <= 0");
            }

            return keyOrder;
        }

        [Obsolete(
            "FetchExpressionForVTs methods moved from TimeSeriesData class because they are used exclusively by availability related functions. They are dubious, however, since SolverExpression uses them in a wrong way")]
        public static string FetchExpressionForVTs(int tims_key)
        {
            string res;
            using (var db = Util.OpenConnection())
            {
                res = FetchExpressionForVTs(tims_key, db);
            }
            return res;
        }

        private static string FetchExpressionForVTs(int timsKey, IDbConnection db)
        {
            Log.Info($"Enter\n\ttims_key = {timsKey}");
            using (var oCmd = new OracleCommand())
            {
                //oCmd.CommandText = "select seqno,valftime,valttime,expression_key from c_definition where calc_key=";
                //oCmd.CommandText += "(select calc_key from rc_column where tims_key=:1 and seqno=1 and rc_key=(select rc_key from timeser where tims_key=:1))";
                //oCmd.Parameters.Add(":1", tims_key);
                const string sql1 =
                    "SELECT D.SeqNo, D.ValFTime, D.ValTTime, D.Expression_Key\n\t" +
                        "FROM TIMESER TS\n\t\t" +
                        "JOIN RC_COLUMN RC ON\n\t\t\t\t" +
                        "RC.Tims_Key = TS.Tims_Key\n\t\t\t" +
                        "AND RC.SeqNo = 1\n\t\t\t" +
                        "AND RC.RC_Key = TS.RC_Key\n\t\t" +
                        "JOIN C_DEFINITION D ON D.Calc_Key = RC.Calc_Key\n\t" +
                        "WHERE TS.Tims_Key = :1"
                    ;
                const string sql2 = "select expression from c_expression where expression_key=:1 order by seqno";
                try
                {

                    oCmd.CommandText = sql1;
                    oCmd.Parameters.Add(":1", timsKey);

                    var tbl = Util.CommandToDataTable(oCmd, db);
                    var sb = new StringBuilder();
                    if (tbl.Rows.Count == 1)
                    {
                        var expressionKey = (int)tbl.Rows[0][3];
                        oCmd.CommandText = sql2;
                        oCmd.Parameters.Clear();
                        oCmd.Parameters.Add(":1", expressionKey);
                        var tbl2 = Util.CommandToDataTable(oCmd, db);
                        foreach (DataRow r in tbl2.Rows)
                        {
                            sb.AppendFormat("{0}\n", r[0].ToString());
                        }
                    }
                    Log.Info($"Exit\n\toCmd.CommandText (1):\n\t{sql1}\n\toCmd.CommandText (2):\n\t{oCmd.CommandText}" +
                             $"\n\tsb (expression):\n\t{sb.ToString()}");
                    return sb.ToString();
                }
                catch (Exception ex)
                {
                    throw new Exception($"FetchExpressionForVTs failed. tims_key = {timsKey}" +
                                        $", oCmd.Parameters.Count = {oCmd.Parameters.Count}" +
                                        $", CommandText (2):\n\t{oCmd.CommandText}", ex);

                }
            }
        }

        public static DataTable FetchInfoForComponentFromTimsKey(int timsKey)
        {
            Log.Info($"Enter\n\tcomp_key = {timsKey}");
            var oCmd = new OracleCommand();
            oCmd.CommandText =
                "select" +
                "\n\t\ta.comp_key, a.hyd_key, b.hydt_key, a.oprt_key, a.tims_key, a.hydp_key, TS.CurveTyp" +
                "\n\tFROM RESTR_COMP_CON a" +
                "\n\t\tJOIN hydcomp b ON a.hyd_key = b.hyd_key" +
                "\n\t\tJOIN TIMESER TS ON TS.Tims_Key = a.Tims_Key" +
                "\n\twhere a.tims_key =: 1" +
                //SIV.Added after discussion with JMA to ensure that the first row with the same Comp_Key, OprT_Key
                //has highest Owner value (usually 1 <=> primary model)
                "\n\torder by a.oprt_key" +
                "\n\t\t, a.Owner DESC"
                ;
            ;
            oCmd.Parameters.Add(null, timsKey);
            Log.Info($"Exit\n\toCmd.CommandText:\n\t{oCmd.CommandText}");
            return Util.CommandToDataTable(oCmd);
        }

        public static DataTable FetchGateInfo(ArrayList hydKeys)
        {
            Log.Info($"Enter\n\thydKeys.Count = {hydKeys.Count}");

            // GATE_NOMFLOW = 6080, 
            var oCmd = new OracleCommand();
            oCmd.CommandText =
                "SELECT ATTR_ID, HYD_KEY, RVAL, CAST(5 AS NUMBER(5)) HYDT_KEY FROM COMPATTR WHERE ATTR_ID = :1 AND HYD_KEY IN (";
            oCmd.Parameters.Add(null, (int)ATTR_TYPE.NOMFLOW);
            if (hydKeys.Count == 0)
            {
                //Add inexistent Hyd_Key:
                oCmd.CommandText += "-1";
            }
            else
            {
                var i = 2;
                foreach (var hydKey in hydKeys)
                {
                    if (i > 2)
                        oCmd.CommandText += ",";
                    oCmd.CommandText += $":{i}";
                    oCmd.Parameters.Add(null, hydKey);
                    i++;
                }
            }
            oCmd.CommandText += ")";
            Log.Info($"Exit\n\toCmd.CommandText:\n\t{oCmd.CommandText}");
            return Util.CommandToDataTable(oCmd);
        }

        public static DataTable FetchGeneratorInfo(int hyd_key)
        {
            Log.Info($"Enter\n\thyd_key = {hyd_key}");
            //		GENERATOR_MINPROD = 3320,
            //		GENERATOR_MAXPROD = 3330,
            //		GENERATOR_NOMPROD = 3340,
            //		GENERATOR_NOMFLOW = 3410, 
            var oCmd = new OracleCommand();
            oCmd.CommandText =
                "select attr_id, hyd_key, rval, CAST(7 AS NUMBER(5)) hydt_key from compattr where attr_id in (:1,:2,:3) and hyd_key in ";
            oCmd.CommandText +=
                "(select a.hyd_keyh from hyrel_ac a, hydcomp b where a.hyd_key=:4 and a.hyd_keyh=b.hyd_key and b.hydt_key=7) order by 1,2";
            oCmd.Parameters.Add(null, (int)ATTR_TYPE.GENERATOR_MINPROD);
            oCmd.Parameters.Add(null, (int)ATTR_TYPE.GENERATOR_MAXPROD);
            oCmd.Parameters.Add(null, (int)ATTR_TYPE.NOMFLOW);
            oCmd.Parameters.Add(null, hyd_key);
            Log.Info($"Exit\n\toCmd.CommandText:\n\t{oCmd.CommandText}");
            return Util.CommandToDataTable(oCmd);
        }

        public static DataTable FetchPumpInfo(int hyd_key)
        {
            Log.Info($"Enter\n\thyd_key = {hyd_key}");
            //		GENERATOR_MINPROD = 3320,
            //		GENERATOR_MAXPROD = 3330,
            //		GENERATOR_NOMPROD = 3340,
            //		GENERATOR_NOMFLOW = 3410, 
            var oCmd = new OracleCommand();
            oCmd.CommandText =
                "select attr_id, hyd_key, rval, CAST(8 AS NUMBER(5)) hydt_key from compattr where attr_id in (:1,:2,:3) and hyd_key in ";
            oCmd.CommandText +=
                "(select a.hyd_keyh from hyrel_ac a, hydcomp b where a.hyd_key=:4 and a.hyd_keyh=b.hyd_key and b.hydt_key=8) order by 1,2";
            oCmd.Parameters.Add(null, (int)ATTR_TYPE.GENERATOR_MINPROD);
            oCmd.Parameters.Add(null, (int)ATTR_TYPE.GENERATOR_MAXPROD);
            oCmd.Parameters.Add(null, (int)ATTR_TYPE.NOMFLOW);
            oCmd.Parameters.Add(null, hyd_key);
            Log.Info($"Exit\n\toCmd.CommandText:\n\t{oCmd.CommandText}");
            return Util.CommandToDataTable(oCmd);
        }

        public static DataTable FetchTsForInternalLocalNetConsum()
        {
            Log.Info("Enter");
            var cmd = new OracleCommand();
            cmd.CommandText =
                "select a.hyd_key,a.attr_id,a.ival,b.filename||b.tscode tscode from compattr a, timeser b where attr_id in (:1,:2) and a.ival=b.tims_key order by hyd_key";
            cmd.Parameters.Add(null, (int)ATTR_TYPE.INTERNAL_CONSUMPTION_LOSS);
            cmd.Parameters.Add(null, (int)ATTR_TYPE.LOCAL_NET_CONSUMPTION);
            Log.Info($"Exit\n\tcmd.CommandText:\n\t{cmd.CommandText}");
            return Util.CommandToDataTable(cmd);
        }

        protected const string revisionsOrderBy = " order by 3,1,2,4"
            ; //SIV. ,4 added to ensure compatibility with DB procedure BulkReadAvailability
        public static DataTable FetchRevisionsForComponent(int comp_key)
        {
            var oCmd = new OracleCommand();
            oCmd.CommandText = "select r.vfrom,r.vto,rs.s_function,rs.value,rs.freq_code,rs.hours,rc.oprt_key ";
            oCmd.CommandText += "from restrictions r, restr_static rs, restr_comp_con rc ";
            oCmd.CommandText += "where r.comp_key=rc.comp_key and r.oprs_key=rs.oprs_key and (rc.comp_key=:1 or hyd_key= ";
            oCmd.CommandText += "(select hydp_key from restr_comp_con where hydp_key is not null and comp_key=:1))" +
                                revisionsOrderBy;
            oCmd.Parameters.Add(null, comp_key);
            return Util.CommandToDataTable(oCmd);
        }

        public static DataTable FetchRevisionsForComponent(int comp_key, int oprt_key)
        {
            OracleCommand oCmd = new OracleCommand();
            oCmd.CommandText = "select r.vfrom,r.vto,rs.s_function,rs.value,rs.freq_code,rs.hours ";
            oCmd.CommandText += "from restrictions r, restr_static rs, restr_comp_con rc ";
            oCmd.CommandText += "where r.comp_key=rc.comp_key and r.oprs_key=rs.oprs_key and and rc.oprt_key=:1 and";
            oCmd.CommandText += "(rc.comp_key=:2 or hyd_key= ";
            oCmd.CommandText += "(select hydp_key from restr_comp_con where hydp_key is not null and comp_key=:2))" +
                                revisionsOrderBy;
            oCmd.Parameters.Add(null, oprt_key);
            oCmd.Parameters.Add(null, comp_key);

            return Util.CommandToDataTable(oCmd);
        }

        public static DataTable FetchRevisionsForComponent(int comp_key, int hydp_key, int oprt_key, int statefilter)
        {
            Log.Info($"Enter\n\tcomp_key = {comp_key}, hydp_key = {hydp_key}, oprt_key = {oprt_key}");
            OracleCommand oCmd = new OracleCommand();
            if ((oprt_key == (int)CompAttribute.Avail || oprt_key == (int)CompAttribute.CAvail) && hydp_key > 0)
            {
                // Include parent component in set, i.e plant if generator and water route if gate
                oCmd.CommandText = "select r.vfrom,r.vto,rs.s_function,rs.value,rs.freq_code,rs.hours ";
                oCmd.CommandText += "from restrictions r, restr_static rs, restr_comp_con rc ";
                if (statefilter == 1)
                {
                    oCmd.CommandText +=
                        "where rs.state=:1 and r.comp_key=rc.comp_key and r.oprs_key=rs.oprs_key and rc.oprt_key=:2 and ";
                    oCmd.CommandText += "((rc.comp_key=:3 and rc.hydp_key=:4) or rc.hyd_key=:5) ";
                    oCmd.CommandText += revisionsOrderBy;

                    oCmd.Parameters.Add(null, statefilter);
                    oCmd.Parameters.Add(null, oprt_key);
                    oCmd.Parameters.Add(null, comp_key);
                    oCmd.Parameters.Add(null, hydp_key);
                    oCmd.Parameters.Add(null,
                        hydp_key); //for cases where hydp_key variable actually is a hyd_key, possibly a station
                                   //the last parameter is repeated for problem with reusing in .net
                }
                else
                {
                    oCmd.CommandText += "where r.comp_key=rc.comp_key and r.oprs_key=rs.oprs_key and rc.oprt_key=:1 and ";
                    oCmd.CommandText += "((rc.comp_key=:2 and rc.hydp_key=:3) or rc.hyd_key=:4) ";
                    oCmd.CommandText += revisionsOrderBy;
                    oCmd.Parameters.Add(null, oprt_key);
                    oCmd.Parameters.Add(null, comp_key);
                    oCmd.Parameters.Add(null, hydp_key);
                    oCmd.Parameters.Add(null,
                        hydp_key); //for cases where hydp_key variable actually is a hyd_key, possibly a station
                                   //the last parameter is repeated for problem with reusing in .net
                }
            }
            else
            {
                oCmd.CommandText = "select r.vfrom,r.vto,rs.s_function,rs.value,rs.freq_code,rs.hours,r.datetim ";
                oCmd.CommandText += "from restrictions r, restr_static rs ";
                if (statefilter == 1)
                {
                    oCmd.CommandText += "where rs.state=:2 and r.comp_key=:1 and r.oprs_key=rs.oprs_key ";
                    oCmd.Parameters.Add(null, statefilter);
                    oCmd.Parameters.Add(null, comp_key);
                }
                else
                {
                    oCmd.CommandText += "where r.comp_key=:1 and r.oprs_key=rs.oprs_key ";
                    oCmd.Parameters.Add(null, comp_key);


                }
                oCmd.CommandText += revisionsOrderBy;
            }
            Log.Info($"Exit\n\toCmd.CommandText:\n\t{oCmd.CommandText}");
            return Util.CommandToDataTable(oCmd);
        }

        public static DataTable FetchRestrictionsForComponent(int comp_key, int oprt_key)
        {
            Log.Info($"Enter\n\tcomp_key = {comp_key}, oprt_key = {oprt_key}");
            var oCmd = new OracleCommand();
            oCmd.CommandText = "select rd.priority, rd.tims_key, r.comp_key ";
            oCmd.CommandText += "from restrictions r, restr_dynamic rd where r.comp_key in ";
            oCmd.CommandText += "(select comp_key from restr_comp_con start with comp_key=:1 ";
            oCmd.CommandText += "connect by prior hyd_key=hydp_key and oprt_key=:2) ";
            oCmd.CommandText +=
                "and r.oprs_key=rd.oprs_key order by 1,2,3"; //SIV. ,2,3 added to ensure compatibility with DB procedure BulkReadAvailability
            oCmd.Parameters.Add(null, comp_key);
            oCmd.Parameters.Add(null, oprt_key);
            Log.Info($"Exit\n\toCmd.CommandText:\n\t{oCmd.CommandText}");
            return Util.CommandToDataTable(oCmd);
        }

        public static double FetchDefaultLimitForComponent(int comp_key)
        {
            // Get more information about the restriction identified by comp_key
            CompAttribute opTypeKey;
            SimCompType hydTypeKey;
            int hydKey;
            int timsKey;
            DataTable tblInfo = FetchInfoForComponent(comp_key);
            if (tblInfo.Rows.Count >= 1) // @@ShareBetweenModels - for now use first. Later - introduce ownership
            {
                hydKey = System.Convert.ToInt32(tblInfo.Rows[0]["hyd_key"]);
                hydTypeKey = (SimCompType)System.Convert.ToInt32(tblInfo.Rows[0]["hydt_key"]);
                opTypeKey = (CompAttribute)System.Convert.ToInt32(tblInfo.Rows[0]["oprt_key"]);
                timsKey = System.Convert.ToInt32(tblInfo.Rows[0]["tims_key"]);
            }
            else
                return double.NaN;

            return FetchDefaultLimitForComponent(hydKey, hydTypeKey, opTypeKey);
        }
  
        public static double FetchDefaultLimitForComponent(int hydKey, SimCompType compType, CompAttribute opTypeKey)
        {
            Log.Info($"Enter\n\thydKey = {hydKey}, compType = {compType}, opTypeKey = {opTypeKey}");
            // Get more information about the restriction identified by hydkey, type and attribute
            // Convert to CompAttribute to attr_id used in COMPATTR table
            // Values found in SimCompConvert.h
            var default_value = double.NaN;
            var getSum = false;
            var getPlantPmin = false;
            var attrId = 0;
            var useHydFunc = false;
            switch (opTypeKey)
            {
                case CompAttribute.Avail:
                    default_value = 1;
                    break;
                case CompAttribute.QMax:
                    if (compType == SimCompType.Gunit)
                        attrId = (int)ATTR_TYPE.NOMFLOW; // Generator nominal flow
                    else if (compType == SimCompType.Gate)
                        attrId = (int)ATTR_TYPE.NOMFLOW; // Gate nominal flow
                    else if (compType == SimCompType.WTR)
                        attrId = (int)ATTR_TYPE.WTR_MXFLOW; // Water route max flow
                    else if (compType == SimCompType.Plant)
                    {
                        attrId = (int)ATTR_TYPE.NOMFLOW; // Generator nom flow
                        getSum = true;
                    }
                    break;
                case CompAttribute.GQMaxMean:
                case CompAttribute.LQMaxMean:
                    default_value = 9999; // Suspect
                    break;
                case CompAttribute.GQMinMean:
                case CompAttribute.LQMinMean:
                case CompAttribute.QBlock:
                case CompAttribute.PBlock:
                case CompAttribute.QMin:
                    default_value = 0;
                    break;
                case CompAttribute.Weight:
                    default_value = 0;
                    break;
                case CompAttribute.Priority:
                    var priorityAttrId = (int)(compType == SimCompType.Gunit ? ATTR_TYPE.GENERATOR_PRIORITY : ATTR_TYPE.GATE_PRIORITY);
                    var configValue = compType == SimCompType.Gunit
#if TSS_Availability
                        ? DefaultGenPriority
                        : DefaultGatePriority;
#else
                        ? (DefaultGenPriority >= 0 
                            ? DefaultGenPriority : IccConfiguration.Restrictions.DefaultGenPriority)
                        : (DefaultGatePriority >= 0 
                            ? DefaultGatePriority :IccConfiguration.Restrictions.DefaultGatePriority);
#endif

                    default_value = FetchDefaultPriority(hydKey, priorityAttrId);
                    Log.Debug($"\n\tdefault_value = {default_value}" +
                        $", configValue = {configValue} (from IccConfiguration.Restrictions.Default)");

                    if (double.IsNaN(default_value))
                    {
                        default_value = configValue;
                    }

                    break;
                case CompAttribute.QVar:
                    default_value = 1000; // Suspect
                    break;
                case CompAttribute.PMax:
                    if (compType == SimCompType.Gunit)
                        attrId = (int)ATTR_TYPE.GENERATOR_MAXPROD; // Generator max prod
                    else if (compType == SimCompType.Plant)
                    {
                        attrId = (int)ATTR_TYPE.GENERATOR_MAXPROD; // Plant max prod
                        getSum = true;
                    }
                    break;
                case CompAttribute.PMin:
                    if (compType == SimCompType.Plant && string.IsNullOrEmpty(UsePminAsDefault))
                    {
                        return 0.0;
                    }

                    default_value = 0;
                    if (compType == SimCompType.Gunit)
                    {
                        attrId = (int)ATTR_TYPE.GENERATOR_MINPROD; // Generator min prod
                    }
                    else if (compType == SimCompType.Plant) // and UsePminAsDefault != null
                    {
                        attrId = (int)ATTR_TYPE.GENERATOR_MINPROD; // Plant min prod
                        getPlantPmin = true;
                    }
                    break;
                case CompAttribute.PVar:
                    default_value = 1000; // Suspect
                    break;
                case CompAttribute.VMax:
                    if (compType == SimCompType.RSV)
                    {
                        attrId = (int)ATTR_TYPE.RESERVOIR_HRV; // Reservoir HRV
                        useHydFunc = true;
                    }
                    break;
                case CompAttribute.VMin:
                    if (compType == SimCompType.RSV)
                        attrId = (int)ATTR_TYPE.RESERVOIR_LRV; // Reservoir LRV
                    break;
                case CompAttribute.RampUp:
                case CompAttribute.RampDown:
                case CompAttribute.VVar:
                    default_value = 1000; // Suspect
                    break;
                case CompAttribute.NReg:
                    default_value = 100; // Suspect
                    break;
                case CompAttribute.Subscr:
                case CompAttribute.SubscrC:
                case CompAttribute.SubscrP:
                    if (compType == SimCompType.Plant)
                    {
                        attrId = (int)ATTR_TYPE.GENERATOR_MAXPROD; // Plant max prod
                        getSum = true;
                    }
                    else if (compType == SimCompType.SubGr)
                        default_value = 9999.0;
                    break;
                case CompAttribute.SubscrPmi:
                    if (compType == SimCompType.Plant)
                    {
                        attrId = (int)ATTR_TYPE.GENERATOR_MINPROD; // Plant min prod
                        getSum = true;
                    }
                    else if (compType == SimCompType.SubGr)
                        default_value = 0.0;
                    break;
                default:
                    default_value = double.NaN;
                    break;
            }

            if (attrId == 0)
                return default_value;

            if (getSum || getPlantPmin)
            {
                var d = FetchNomValue(hydKey, attrId, getPlantPmin);
                return double.IsNaN(d) ? default_value : d;
            }

            var oCmd = new OracleCommand();
            if (useHydFunc)
            {
                oCmd.CommandText =
                    "select max(fn_val) from hydfn_vl where fn_key in (select fn_key from hydco_fn where hyd_key = :1 and hydf_key = 10000)";
                oCmd.Parameters.Add(null, hydKey);
            }
            else
            {
                oCmd.CommandText = "select rval from compattr where hyd_key=:1 and attr_id=:2";
                oCmd.Parameters.Add(null, hydKey);
                oCmd.Parameters.Add(null, attrId);
            }
            var tbl = Util.CommandToDataTable(oCmd);
            Log.Info($"Exit\n\toCmd.CommandText:\n\t{oCmd.CommandText}");
            if (tbl.Rows.Count >= 1) 
                // To compensate for an error in COMPATTR introduced from SIM, multiple rows with same contents 
                return System.Convert.ToDouble(tbl.Rows[0][0]);

            return double.NaN;
        }
        private static double FetchDefaultPriority(int hydKey, int attr_id)
        {
            var oCmd = new OracleCommand();
            oCmd.CommandText = "select ival from compattr where hyd_key=:1 and attr_id=:2";
            oCmd.Parameters.Add(null, hydKey);
            oCmd.Parameters.Add(null, attr_id);

            var tbl = Util.CommandToDataTable(oCmd);
            if (tbl.Rows.Count >= 1) 
                // To compensate for an error in COMPATTR introduced from SIM, multiple rows with same contents 
                return System.Convert.ToDouble(tbl.Rows[0][0]);

            return double.NaN;
        }

        public static DataTable FetchInfoForComponent(int comp_key)
        {
            Log.Info($"Enter\n\tcomp_key = {comp_key}");
            var oCmd = new OracleCommand();
            oCmd.CommandText =
                "select" +
                "\n\t\ta.comp_key, a.hyd_key, b.hydt_key, a.oprt_key, a.tims_key, a.hydp_key, TS.CurveTyp" +
                "\n\tFROM RESTR_COMP_CON a" +
                "\n\t\tJOIN hydcomp b ON a.hyd_key = b.hyd_key" +
                "\n\t\tJOIN TIMESER TS ON TS.Tims_Key = a.Tims_Key" +
                "\n\twhere a.comp_key =: 1" +
                "\n\torder by a.oprt_key" +
                //SIV.Added after discussion with JMA to ensure that the first row with the same Comp_Key, OprT_Key
                //has highest Owner value (usually 1 <=> primary model)
                "\n\t\t, a.Owner DESC"
                ;
            oCmd.Parameters.Add(null, comp_key);
            Log.Info($"Exit\n\toCmd.CommandText:\n\t{oCmd.CommandText}");
            return Util.CommandToDataTable(oCmd);
        }

        public static double FetchNomValue(int hydKey, int attrId, bool getPlantPmin)
        {
            var cmd = new OracleCommand();
            cmd.CommandText = "select a.hyd_key,a.hydt_key,b.hyd_keyh achyd_key,c.rval ";
            cmd.CommandText += "from hydcomp a, hyrel_ac b, compattr c ";
            cmd.CommandText += "where a.hyd_key =:1 and a.hyd_key = b.hyd_key and b.hyd_keyh = c.hyd_key and c.attr_id=:2";
            cmd.Parameters.Add(null, hydKey);
            cmd.Parameters.Add(null, attrId);
            var dt = Util.CommandToDataTable(cmd);
            double v = 0;
            if (dt.Rows.Count == 0)
                return double.NaN;

            if (getPlantPmin) //The result is the minimum of plant individual generator attributes (Pmin)
            {
                v = double.MaxValue;
                foreach (DataRow r in dt.Rows)
                {
                    var temp = Convert.ToDouble(r["RVAL"]);
                    if (temp < v)
                        v = temp;
                }
            }
            else //for other attributes (Pmax, Qmax, SubscrP, SubscrPmi) we calculate the sum of individual generator attributes:
            {
                foreach (DataRow r in dt.Rows)
                {
                    v += Convert.ToDouble(r["RVAL"]);
                }
            }
            return v;
        }

        public static DataTable FetchStructureInfoForComponent(int hyd_key, int oprt_key)
        {
            Log.Info($"Enter\n\thyd_key = {hyd_key}, oprt_key = {oprt_key}");
            var oCmd = new OracleCommand();
            oCmd.CommandText =
                "select a.hyd_key,a.hydp_key,a.tims_key,a.oprt_key,b.hydt_key from restr_comp_con a,hydcomp b " +
                "where a.hydp_key=:1 and a.oprt_key=:2 and a.hyd_key=b.hyd_key\n\t" +
                "ORDER BY Hyd_Key, Tims_Key" + //SIV. ORDER BY added for test compatibility with ICC_Sim.BulkReadAvailability
                "";

            oCmd.Parameters.Add(null, hyd_key);
            oCmd.Parameters.Add(null, oprt_key);
            Log.Info($"Exit\n\toCmd.CommandText:\n\t{oCmd.CommandText}");
            return Util.CommandToDataTable(oCmd);
        }

        public static int FetchTimsKeyForNetSubscription(int comp_key)
        {
            Log.Info($"Enter\n\tcomp_key = {comp_key}");
            var oCmd = new OracleCommand()
            {
                CommandText =
                    "SELECT A.TIMS_KEY FROM RESTR_COMP_CON A WHERE A.OPRT_KEY = :1 AND A.HYD_KEY IN\n\t" +
                    "(SELECT B.HYD_KEY FROM RESTR_COMP_CON B WHERE B.COMP_KEY = :2)"
            };
            oCmd.Parameters.Add(null, (int)CompAttribute.Subscr);
            oCmd.Parameters.Add(null, comp_key);
            var tbl = Util.CommandToDataTable(oCmd);

            Log.Info($"Exit\n\toCmd.CommandText:\n\t{oCmd.CommandText}\n\ttbl.Rows.Count = {tbl.Rows.Count}");

            if (tbl.Rows.Count >= 1) // @@ShareBetweenModels - Different hyd_keys, else equal
                return (int)tbl.Rows[0][0];

            return 0;
        }

        public static DataTable FetchCompAttributes()
        {
            Profiler.Log(5000 + 40, "FetchCompAttributes, SELECT");
            var oCmd = new OracleCommand();
            oCmd.CommandText =
                "SELECT * FROM OPRRS_TP WHERE OPRT_KEY >= 12000"
                //"SELECT CA.OPRT_KEY, CA.CODE ,CA.NAME ,CA.DESCRIPT" +
                //    "\n\tFROM ICC_LANGUAGE L" +
                //    "\n\tJOIN TP_OPRRS_TP CA ON CA.LANGUAGE_KEY = L.LANGUAGE_KEY" +
                //        "\n\t\tAND CA.OPRT_KEY >= 12000" +
                //    "\n\tWHERE L.LANGUAGE_NAME = PDMISC.NLS_SESSION_LANGUAGE" //'AMERICAN'
                //"SELECT CA.OPRT_KEY, CA.CODE, CA.NAME ,CA.DESCRIPT" +
                //    "\n\tFROM TP_OPRRS_TP CA" +
                //    "\n\tWHERE CA.OPRT_KEY >= 12000" +
                //        "\n\t\tAND CA.LANGUAGE_KEY = (SELECT MIN(L.LANGUAGE_KEY) FROM ICC_LANGUAGE L)"
                ;
            var table = Util.CommandToDataTable(oCmd);
            Profiler.Log(5000 + 44, "FetchCompAttributes, SELECT. Exit");
            return table;
        }

        public static void FetchCompAttributes(out List<Tuple<int, string, string, string>> attrTuples)
        {
            Profiler.Log(5000 + 0, "FetchCompAttributes, DB proc");
            attrTuples = new List<Tuple<int, string, string, string>>(30);
            using (var cmd = new OracleCommand())
            {
                Profiler.Log(5000 + 10);
                cmd.CommandType = CommandType.StoredProcedure;
                cmd.CommandText = /*"ICC_Sim.*/"FetchCompAttributes";

                var parRcCompAttributes = cmd.Parameters.Add("rcCompAttributes", OracleDbType.RefCursor);
                parRcCompAttributes.Direction = ParameterDirection.Output;

                using (var conn = new OracleConnection())
                {
                    //var stopwatch = Stopwatch.StartNew();
                    Util.PrepareConnection(conn);
                    Log.Warn($"\n\tcmd.Connection.State = {conn.State}, ConnectionString:\n\t{conn.ConnectionString}");
                    //stopwatch.Stop();
                    //var openConnElapsed = stopwatch.ElapsedMilliseconds;
                    //stopwatch.Reset(); stopwatch.Start();
                    Util.ExecuteCommand(cmd, conn);
                    //stopwatch.Stop();
                    //var fetchComAttrsElapsed = stopwatch.ElapsedMilliseconds;
                    //stopwatch.Reset(); stopwatch.Start();
                    Profiler.Log(5000 + 30, "FetchCompAttributes. rdCompAttributes.Read");
                    var rcCompAttributes = (OracleRefCursor)parRcCompAttributes.Value;
                    var compAttrCount = 0;
                    rcCompAttributes.FetchSize = 200;
                    var rdCompAttributes = rcCompAttributes.GetDataReader();
                    while (rdCompAttributes.Read())
                    {
                        var oprtKey = rdCompAttributes["OPRT_KEY"] != DBNull.Value ? Convert.ToInt32(rdCompAttributes["OPRT_KEY"]) : -1;
                        var code = rdCompAttributes["CODE"] != DBNull.Value ? Convert.ToString(rdCompAttributes["CODE"]) : "";
                        var name = rdCompAttributes["NAME"] != DBNull.Value ? Convert.ToString(rdCompAttributes["NAME"]) : "";
                        var descript = rdCompAttributes["DESCRIPT"] != DBNull.Value ? Convert.ToString(rdCompAttributes["DESCRIPT"]) : "";
                        attrTuples.Add(Tuple.Create(oprtKey, code, name, descript));
                        compAttrCount++;
                        //    Log.Warn($"\n{MethodBase.GetCurrentMethod().Name}" +
                        //        $"\n\t{compAttrCount}. {oprtKey}, {code}, {name}, {descript}");
                    }
                    //stopwatch.Stop();
                    //var readComAttrsElapsed = stopwatch.ElapsedMilliseconds;
                    //    Log.Warn($"\n{MethodBase.GetCurrentMethod().Name}" +
                    //        $"\n\topenConnElapsed = {openConnElapsed}, fetchComAttrsElapsed = {fetchComAttrsElapsed}" +
                    //        $", readComAttrsElapsed = {readComAttrsElapsed}, compAttrCount = {compAttrCount}" +
                    //        $"{Util.LogSeparator}");
                }
            }
            Profiler.Log(5000 + 31, "FetchCompAttributes, DB proc. Exit");
        }
    }
}
