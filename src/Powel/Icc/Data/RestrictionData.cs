using System;
using System.Data;
using System.Text;
using System.Collections;
using Oracle.ManagedDataAccess.Client;
using Powel.Icc.Common;
using System.Collections.Generic;
using System.Diagnostics;
using log4net;
using Oracle.ManagedDataAccess.Types;
using System.Reflection;
//using System.Runtime.InteropServices.ComTypes;

namespace Powel.Icc.Data
{
    /// <summary>
    /// Summary description for Datalayer.
    /// </summary>
    public class RestrictionData: RestrictionDataRO
    {


        public RestrictionData()
        {
        }

    #region NotInUseMethods

    /// <summary>
    /// Section for common methods
    /// </summary>
    /// <returns></returns>
    //		public static int FetchKeyTabRcount() 
    //		{
    //			return Convert.ToInt32(Util.CommandToScalar(new OracleCommand("select keytab_seq.nextval from dual")));
    //		}
    //		public DataSet FetchSystemHydComps(int prdKey) 
    //		{
    //			OracleCommand oCmd = new OracleCommand();
    //			oCmd.CommandText = "select hyd_key,hydt_key,name from hydcomp where hyd_key in ";
    //			oCmd.CommandText += "(SELECT HYDCOMP.HYD_KEY FROM HYDCOMP, HYREL_AC, PRRUN_DS WHERE hydcomp.watc_key = prrun_ds.watc_key and ";
    //			oCmd.CommandText += "(HYREL_AC.HYD_KEY = HYDCOMP.HYD_KEY OR HYREL_AC.HYD_KEYH = HYDCOMP.HYD_KEY) ";
    //			oCmd.CommandText += "and hydt_key in (2,4,5,6,7) AND (PRRUN_DS.PRD_KEY = :1))";
    //			oCmd.Parameters.Add(null, prdKey);
    //			DataSet ds = new DataSet();
    //			Util.CommandIntoDataSet(oCmd,ds,"SystemHydcomps");
    //			return ds;
    //		}
    //		public static int FetchWaterCourseKeyForGate(int gateKey) 
    //		{
    //			OracleCommand oCmd = new OracleCommand();
    //			oCmd.CommandText = "select distinct hyd_key,name from hydcomp where hydt_key=2 and ";
    //			oCmd.CommandText +=	"hyd_key in (select hyd_key from hyrel_ac where hyd_keyh=:1)";
    //			oCmd.Parameters.Add(null,gateKey);
    //			return Convert.ToInt32(Util.CommandToScalar(oCmd));
    //		}
    //		public static DataTable FetchGatesForWaterCourse(int hydKey) 
    //		{
    //			OracleCommand oCmd = new OracleCommand();
    //			oCmd.CommandText = "select distinct hyd_key, name from hydcomp where hydt_key=5 and ";
    //			oCmd.CommandText +=	"hyd_key in (select hyd_keyh from hyrel_ac where hyd_key=:1)";
    //			oCmd.Parameters.Add(null,hydKey);
    //			return Util.CommandToDataTable(oCmd);
    //		}
    //		public static DataTable FetchGeneratorsForStation(int hydKey) 
    //		{
    //			OracleCommand oCmd = new OracleCommand();
    //			oCmd.CommandText = "select distinct hyd_key, name from hydcomp where hydt_key=7 and ";
    //			oCmd.CommandText +=	"hyd_key in (select hyd_keyh from hyrel_ac where hyd_key=:1)";
    //			oCmd.Parameters.Add(null,hydKey);
    //			return Util.CommandToDataTable(oCmd);
    //		}

    #endregion

    #region LoadMethods
    public static DataTable LoadAllDatasets()
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select * from ( select watercou.watc_key, prrun_ds.prd_key, watercou.name wname, prrun_ds.name pname ";
        oCmd.CommandText += "from prrun_ds, watercou where prrun_ds.watc_key = watercou.watc_key order by 3,4)";
        return Util.CommandToDataTable(oCmd);
        ;
    }

    public static DataTable LoadAllDatasets(string systemGroupName)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select * from ( select watercou.watc_key, prrun_ds.prd_key, watercou.name wname, prrun_ds.name pname ";
        oCmd.CommandText += "from prrun_ds, watercou where prrun_ds.watc_key = watercou.watc_key minus ";
        oCmd.CommandText +=
            "select simsysda.watc_key, simsysda.prd_key, watercou.name wname, prrun_ds.name pname from simsysda, ";
        oCmd.CommandText += "prrun_ds,watercou where watercou.watc_key = simsysda.watc_key and ";
        oCmd.CommandText += "simsysda.prd_key = prrun_ds.prd_key  and (simsysda.name = :1)";
        oCmd.CommandText += "order by 3,4)";
        oCmd.Parameters.Add(null, systemGroupName);
        return Util.CommandToDataTable(oCmd);
        ;
    }

    #endregion

    #region DeleteMethods

    public static void DeleteRestrAttributes(int attrKey)
    {
        //int languageKey = -1;
        var cmd = new OracleCommand();
        //			Object o2 = Util.SqlToScalar("select language_key from icc_language where language_name=pdmisc.nls_session_language");		
        //			if (o2 == null) return;
        //			languageKey = Convert.ToInt32(o2);
        cmd.CommandText = "delete from tp_restr_attributes_tp  where seqno = 1 and attr_key = " + attrKey.ToString();
        //			cmd.CommandText += " and language_key = " + languageKey.ToString();
        Util.ExecuteCommand(cmd);
    }

    public static int DeleteRestrictionForExternalID(string externalId, string externalSource)
    {
        // changed return value from bool to int. Where int value is comp_key to current deleted row.
        // for updating object_changed.
        int compKey = -1;
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "select oprs_key,comp_key from restrictions where external_id = :1 and external_source = :2";
        cmd.Parameters.Add(null, externalId);
        cmd.Parameters.Add(null, externalSource);
        DataTable dt = Util.CommandToDataTable(cmd);
        if (dt.Rows.Count == 0) return compKey;
        foreach (DataRow dr in dt.Rows)
        {
            DeleteCurrentRevision((int) dr["OPRS_KEY"]);
            compKey = (int) dr["COMP_KEY"];
        }
        return compKey;
    }

    public static bool RemoveTsConsumeCompAttr(int hydKey, int timsKey, ATTR_TYPE attrType)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "delete from compattr where hyd_key=:1 and attr_id=:2 and ival=:3";
        cmd.Parameters.Add(null, hydKey);
        cmd.Parameters.Add(null, (int) attrType);
        cmd.Parameters.Add(null, timsKey);
        Util.ExecuteCommand(cmd);
        return true;
    }

    public static bool RemoveTsConsumeCompAttr(int compKey, int hydKey)
    {
        OracleCommand cmd = new OracleCommand();
        DataTable dt =
            Util.CommandToDataTable(new OracleCommand("select oprs_key from restrictions where comp_key = " + compKey));
        if (dt.Rows.Count == 0)
        {
            cmd.CommandText = "delete from compattr where hyd_key=:1 and attr_id in (:2,:3)";
            cmd.Parameters.Add(null, hydKey);
            cmd.Parameters.Add(null, (int) ATTR_TYPE.INTERNAL_CONSUMPTION_LOSS);
            cmd.Parameters.Add(null, (int) ATTR_TYPE.LOCAL_NET_CONSUMPTION);
            Util.ExecuteCommand(cmd);
        }
        return true;
    }

    public static void DeleteSubscruptionGroupDefinition(int hydKeyh)
    {
        Util.ExecuteCommand(new OracleCommand("delete from hyrel_ac where hyretype=3 and hyd_keyh = " + hydKeyh));
    }

    public static void DeleteSubscriptionComponent(int hydKey)
    {
        Util.ExecuteSql("delete from hydcomp where hydt_key=23 and hyd_key=" + hydKey);
    }

    public static void DeleteGroupInSimsysda(string groupName)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "delete from simsysda where upper(name)=upper(:1)";
        oCmd.Parameters.Add(null, groupName);
        Util.ExecuteCommand(oCmd);
    }

    [Obsolete("use variant with idbconnection argument")]
    public static void DeleteCurrentRestrDynamic(int oprsKey, short priority, int timsKey)
    {
        DeleteCurrentRestrDynamic(oprsKey, priority, timsKey, null);
    }

    public static void DeleteCurrentRestrDynamic(int oprsKey, short priority, int timsKey, IDbConnection connection)
    {
        var countCmd = new OracleCommand()
        {
            CommandText = "select count(*) from restr_dynamic where oprs_key = :1"
        };
        countCmd.Parameters.Add(":1", oprsKey);
        var count = Convert.ToInt32(Util.CommandToScalar(countCmd));
        var cmd = new OracleCommand();
        if (count == 1)
        {
            cmd.CommandText = @"
				begin
					delete from restr_dynamic where oprs_key=:1;
					delete from restrictions_tab where oprs_key = :2;
				end;";
            cmd.Parameters.Add(":1", oprsKey);
            cmd.Parameters.Add(":2", oprsKey);
        }
        else
        {
            cmd.CommandText = @"
				begin
					delete from restr_dynamic where oprs_key=:1 and priority=:2;
				end;";
            cmd.Parameters.Add(":1", oprsKey);
            cmd.Parameters.Add(":2", priority);
        }
        Util.ExecuteCommand(cmd);
        TimeSeriesData.DeleteTimeSeries(timsKey, connection);
    }

    public static void DeleteCurrentRevision(int oprsKey)
    {
        var oCmd = new OracleCommand();
        oCmd.CommandText = @"
            begin
	            delete from restrictions_tab where oprs_key=:1;
	            delete from restr_static where oprs_key=:1;
	            delete from restr_attributes where oprs_key=:1;
            end;";
        oCmd.Parameters.Add(":1", oprsKey);
        Util.ExecuteCommand(oCmd);
    }

    #endregion

    #region SimsysdaMethods

    public static DataTable SimsysdaIsNameTaken(string sysGroupName)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "select name from simsysda where upper(name)=upper(:1)";
        oCmd.Parameters.Add(null, sysGroupName);
        return Util.CommandToDataTable(oCmd);
    }

    #endregion

    #region UpdateMethods

    /// <summary>
    /// Section for methods for Revision part of the data layer
    /// </summary>
    /// <param name="sysGroupName"></param>
    /// <param name="rCount"></param>
    /// <returns></returns>
    public static int UpdateIntoRestrictionsAdvance(int oprsKey, int compKey, string description)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "update restrictions_tab set comp_key = :1, description = :2 where oprs_key=:3";
        oCmd.Parameters.Add(null, compKey);
        oCmd.Parameters.Add(null, description);
        oCmd.Parameters.Add(null, oprsKey);
        Util.ExecuteCommand(oCmd);
        return oprsKey;
    }

    public static int UpdateIntoRestrictions(int oprsKey, int compKey, DateTime vfrom, DateTime vto, string description)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "update restrictions_tab set comp_key = :1, vfrom = :2, vto = :3, description = :4 where oprs_key=:5";
        oCmd.Parameters.Add(null, compKey);
        oCmd.Parameters.Add(null, vfrom);
        oCmd.Parameters.Add(null, vto);
        oCmd.Parameters.Add(null, description);
        oCmd.Parameters.Add(null, oprsKey);
        Util.ExecuteCommand(oCmd);
        return oprsKey;
    }

    public static int UpdateIntoRestrictions(int oprsKey, int compKey, DateTime vfrom, DateTime vto, string description,
        string externalId, string externalSource)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "update restrictions_tab set comp_key = :1, vfrom = :2, vto = :3, description = :4, ";
        oCmd.CommandText += "external_id = :5, external_source = :6 where oprs_key=:7";
        oCmd.Parameters.Add(null, compKey);
        oCmd.Parameters.Add(null, vfrom);
        oCmd.Parameters.Add(null, vto);
        oCmd.Parameters.Add(null, description);
        oCmd.Parameters.Add(null, externalId);
        oCmd.Parameters.Add(null, externalSource);
        oCmd.Parameters.Add(null, oprsKey);
        Util.ExecuteCommand(oCmd);
        return oprsKey;
    }

    public static int UpdateIntoRestrictions(int oprsKey, int compKey, DateTime vfrom, DateTime vto, string description,
        string externalId, string externalSource, string externalId2, string externalSource2)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "update restrictions_tab set comp_key = :1, vfrom = :2, vto = :3, description = :4, ";
        oCmd.CommandText +=
            "external_id = :5, external_source = :6,external_id2 = :7, external_source2 = :8 where oprs_key=:9";
        oCmd.Parameters.Add(null, compKey);
        oCmd.Parameters.Add(null, vfrom);
        oCmd.Parameters.Add(null, vto);
        oCmd.Parameters.Add(null, description);
        oCmd.Parameters.Add(null, externalId);
        oCmd.Parameters.Add(null, externalSource);
        oCmd.Parameters.Add(null, externalId2);
        oCmd.Parameters.Add(null, externalSource2);
        oCmd.Parameters.Add(null, oprsKey);
        Util.ExecuteCommand(oCmd);
        return oprsKey;
    }


    public static void UpdateIntoRestrStatic(int oprsKey, short sFunc, double val, short freqKey, short hours)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "update restr_static set s_function = :1, value = :2, freq_code = :3, hours = :4 where oprs_key = :5";
        oCmd.Parameters.Add(null, sFunc);
        oCmd.Parameters.Add(null, val);
        oCmd.Parameters.Add(null, freqKey);
        oCmd.Parameters.Add(null, hours);
        oCmd.Parameters.Add(null, oprsKey);
        Util.ExecuteCommand(oCmd);
    }

    public static void UpdateIntoRestrStatic(int oprsKey, short sFunc, double val, short freqKey, short hours,
        short state)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "update restr_static set s_function = :1, value = :2, freq_code = :3, hours = :4, state = :5 where oprs_key = :6";
        oCmd.Parameters.Add(null, sFunc);
        oCmd.Parameters.Add(null, val);
        oCmd.Parameters.Add(null, freqKey);
        oCmd.Parameters.Add(null, hours);
        oCmd.Parameters.Add(null, state);
        oCmd.Parameters.Add(null, oprsKey);
        Util.ExecuteCommand(oCmd);
    }

    public static void UpdateComponentAttributeHydParentKey(RestrictionData.RestrCompCon con)
    {
        OracleCommand oICmd = new OracleCommand();
        oICmd.CommandText = "update restr_comp_con set hydp_key=:1 where comp_key=:2";
        oICmd.Parameters.Add(null, con.hydp_key); // :1
        oICmd.Parameters.Add(null, con.comp_key); // :2
        Util.ExecuteCommand(oICmd);
    }

    public static void UpdateTimeSeriesCurveType(int timsKey, CurveType curveType)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "update timeser set curvetyp = :1 where tims_key = :2";
        cmd.Parameters.Add(null, (short) curveType);
        cmd.Parameters.Add(null, timsKey);
        Util.ExecuteCommand(cmd);
    }

    public static void UpdateRestrAttributes(int attrKey, String name, String description)
    {
        OracleCommand cmd = new OracleCommand();
        Object o2 = Util.SqlToScalar(
            "select language_key from icc_language where language_name=pdmisc.nls_session_language");
        if (o2 == null) return;
        int languageKey = Convert.ToInt32(o2);
        cmd.CommandText = "update tp_restr_attributes_tp set NAME = :1, DESCRIPTION = :2 ";
        cmd.CommandText += "where ATTR_KEY = :3 and language_key = :4";
        cmd.Parameters.Add(null, name);
        cmd.Parameters.Add(null, description);
        cmd.Parameters.Add(null, attrKey);
        cmd.Parameters.Add(null, languageKey);
        Util.ExecuteCommand(cmd);
    }

    #endregion

    #region GetMethods

    public static DataTable GetAllConnectedTimeSeriesForObject(int hydKey, int hydtKey)
    {
        DataTable dt = new DataTable("DtAllConnectedTimeSeriesForObject");
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText =
            "select distinct d.wname||'.'||d.pname simname,d.prd_key,d.watc_key,b.hyd_key,a.hname hydname,b.oprt_key,b.name OprtName,b.tims_key,b.tscode,b.owner,b.comp_key ";
        cmd.CommandText +=
            "from view_hydcomp_in_sim_model a, view_restr_comp_con b, view_restr_comp_con c, view_hydcomp_in_sim_model d ";
        cmd.CommandText += "where a.hyd_key=b.hyd_key and b.owner in (0,1) and (b.hyd_key = :1 ";
        if (hydtKey == (int) SimCompType.Plant)
            cmd.CommandText += "or b.hydp_key = :2";
        cmd.CommandText += ") and c.comp_key = b.comp_key and c.owner=1 and c.hyd_key=d.hyd_key order by a.hname";
        cmd.Parameters.Add(null, hydKey);
        if (hydtKey == (int) SimCompType.Plant)
            cmd.Parameters.Add(null, hydKey);
        return dt = Util.CommandToDataTable(cmd);
    }

    public static DataTable GetAllRestrTypesForHydkey(int hydKey)
    {
        DataTable dt = new DataTable("DtAllRestrTypesForHydkey");
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText =
            "select oprt_key,name from view_restr_comp_con where hyd_key=:1 and owner=1 order by oprt_key";
        cmd.Parameters.Add(null, hydKey);
        return dt = Util.CommandToDataTable(cmd);
    }

    public static DataTable GetRestrCompConForHydOprtKey(int hydKey, int oprtKey)
    {
        DataTable dt = new DataTable("DtRestrCompConForHydOprtKey");
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "select * from view_restr_comp_con where hyd_key=:1 and oprt_key=:2 order by owner";
        cmd.Parameters.Add(null, hydKey);
        cmd.Parameters.Add(null, oprtKey);
        return dt = Util.CommandToDataTable(cmd);
    }

    public static void InsertIntoRestrCompCon(int comp_key, int hyd_key, int hydp_key, int oprt_key, int tims_key,
        short owner)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "insert into restr_comp_con(comp_key,hyd_key,";
        if (hydp_key != -1)
        {
            cmd.CommandText += "hydp_key,oprt_key,tims_key,owner) ";
            cmd.CommandText += "values(:1,:2,:3,:4,:5,:6)";
        }
        else
        {
            cmd.CommandText += "oprt_key,tims_key,owner) ";
            cmd.CommandText += "values(:1,:2,:3,:4,:5)";
        }
        cmd.Parameters.Add(null, comp_key);
        cmd.Parameters.Add(null, hyd_key);
        if (hydp_key != -1)
            cmd.Parameters.Add(null, hydp_key);
        cmd.Parameters.Add(null, oprt_key);
        cmd.Parameters.Add(null, tims_key);
        cmd.Parameters.Add(null, owner);
        Util.CommandToScalar(cmd);
    }

    public static void UpdateOwnerInRestrCompCon(int comp_key, short newOwner, int hyd_key, int oprt_key)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "update restr_comp_con set owner=:1 where hyd_key=:2 and comp_key=:3 and oprt_key=:4";
        cmd.Parameters.Add(null, newOwner);
        cmd.Parameters.Add(null, hyd_key);
        cmd.Parameters.Add(null, comp_key);
        cmd.Parameters.Add(null, oprt_key);
        Util.CommandToScalar(cmd);
    }

    public static void UpdateOwnerInRestrCompCon(int newCompKey, int oldCompKey, int tims_key, short newOwner,
        int hyd_key, int oprt_key)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText =
            "update restr_comp_con set owner=:1, comp_key=:2, tims_key=:3 where hyd_key=:4 and comp_key=:5 and oprt_key=:6";
        cmd.Parameters.Add(null, newOwner);
        cmd.Parameters.Add(null, newCompKey);
        cmd.Parameters.Add(null, tims_key);
        cmd.Parameters.Add(null, hyd_key);
        cmd.Parameters.Add(null, oldCompKey);
        cmd.Parameters.Add(null, oprt_key);
        Util.CommandToScalar(cmd);
    }

    public static DataTable GetGeneratorsForPlant(int plantKey)
    {
        DataTable dt = new DataTable("DtGeneratorsForPlant");
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "select * from view_sim_plant_generators where plant_key=:1";
        cmd.Parameters.Add(null, plantKey);
        return dt = Util.CommandToDataTable(cmd);
    }

    public static DataTable GetSimObjectByType(int prdKey, int hydtKey)
    {
        DataTable dt = new DataTable("DtSimObjectByType");
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText =
            "select distinct wname||'.'||pname SIMNAME,hname HYDNAME,code,hyd_key,prd_key,watc_key,hydt_key ";
        cmd.CommandText += "from view_hydcomp_in_sim_model where prd_key=:1 and hydt_key=:2 order by hname";
        cmd.Parameters.Add(null, prdKey);
        cmd.Parameters.Add(null, hydtKey);
        return dt = Util.CommandToDataTable(cmd);
    }

    public static DataTable GetSimObjectByName(string name)
    {
        DataTable dt = new DataTable("DtSimObjectByName");
        OracleCommand cmd = new OracleCommand();
        //			cmd.CommandText = "select distinct * from view_hydcomp_in_sim_model where hname like :1";
        cmd.CommandText =
            "select distinct wname||'.'||pname fullname,hname,code from view_hydcomp_in_sim_model where hname like :1";
        cmd.Parameters.Add(null, name);
        return dt = Util.CommandToDataTable(cmd);
    }

    public static DataTable GetAllSimModels()
    {
        DataTable dt = new DataTable("DtAllSimModels");
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText =
            "select distinct NAME_SYSTEM||'.'||NAME_DATASET fullname, watc_key,prd_key from view_sim_dataset order by fullname";
        return dt = Util.CommandToDataTable(cmd);
    }

    public static DataTable GetAllComponentInSimModel(int prdKey)
    {
        DataTable dt = new DataTable("DtAllComponentsSimModel");
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "select distinct * from view_hydcomp_in_sim_model where prd_key=:1 order by hydt_key,hname";
        cmd.Parameters.Add(null, prdKey);
        return dt = Util.CommandToDataTable(cmd);
    }

    public static int GetOprsKeyForCompKeyAndExternalId(int compKey, string externalId, string externalSource)
    {
        var cmd = new OracleCommand();
        cmd.CommandText =
            "select oprs_key from restrictions where comp_key = :1 and external_id = :2 and external_source = :3";
        cmd.Parameters.Add(null, compKey);
        cmd.Parameters.Add(null, externalId);
        cmd.Parameters.Add(null, externalSource);
        var obj = Util.CommandToScalar(cmd);

        if (obj == null)
            return 0;

        return Convert.ToInt32(obj);
    }

    //[Obsolete("Use variant with with idbconnection instead")]
    //public static RestrictionOwner GetComponentAttributeId(RestrictionData.RestrCompCon con, bool create)
    //{
    //	using(var connection = Util.OpenConnection())
    //	{
    //		return GetComponentAttributeId(con, create, connection);
    //	}
    //}

    public static RestrictionOwner GetComponentAttributeId(
        RestrictionData.RestrCompCon con, bool create, IDbConnection connection)
    {
        if (create)
        {
            var ts = new TimeSeriesDataRO.TimeSeriesInfo();
            ts.tscode = TimeSeriesData.CreateName(con);
            ts.Resolution = TimeResolution.Unconstrained;
            ts.descript = "Automatically created from RestrCompCon info";
            ts.tser_key = (int) TsType.Availability;
            ts.tsor_key = (int) TsOrigin.Calculated; // Any better ?
            ts.tspr_key = (int) TsProducer.ICC_Avail;
            ts.CurveType = CurveType.Step;
            return GetComponentAttributeId(con, ts, create, connection);
        }

        return GetComponentAttributeId(con, null, create, connection);
    }

    //[Obsolete("Use variant with idbconnection instead")]
    //public static RestrictionOwner GetComponentAttributeId(RestrictionData.RestrCompCon con, TimeSeriesData.TimeSeriesInfo ts, bool create) {
    //	using(IDbConnection connection =  Util.OpenConnection())
    //	{
    //		return GetComponentAttributeId(con, ts, create, connection);
    //	}
    //} 

    public static RestrictionOwner GetComponentAttributeId(
        RestrictionData.RestrCompCon con, TimeSeriesDataRO.TimeSeriesInfo ts, bool create, IDbConnection connection)
    {
        var oCmd = new OracleCommand();
        oCmd.CommandText =
            "select comp_key,tims_key,hydp_key,owner from restr_comp_con where hyd_key=:1 and oprt_key=:2 order by owner";
        oCmd.Parameters.Add(":1", OracleDbType.Int32, con.hyd_key, ParameterDirection.Input); // ":1", hyd_key);
        oCmd.Parameters.Add(":2", OracleDbType.Int32, con.oprt_key, ParameterDirection.Input); //":2", oprt_key);
        DataTable tbl = Util.CommandToDataTable(oCmd);
        bool exist = false;

        if (tbl.Rows.Count == 1)
        {
            // Does ts realy exist?
            int tims_key = Convert.ToInt32(tbl.Rows[0][1]);
            OracleCommand oCmdts = new OracleCommand();
            oCmdts.CommandText = "select tims_key from timeser where tims_key=:1";
            oCmdts.Parameters.Add(":1", OracleDbType.Int32, tims_key, ParameterDirection.Input);
            DataTable tblts = Util.CommandToDataTable(oCmdts);
            if (tblts.Rows.Count == 1) exist = true;
        }

        if (tbl.Rows.Count == 0 || (tbl.Rows.Count == 1 && !exist)) // Does not exist ?
        {
            if (create && ts != null) // Create time series ?
            {
                ts.tscode = TimeSeriesData.EnsureUniqeName(ts.tscode, connection);
                TimeSeriesData.CreateTimeSeries(ts, "## = ,", connection);
                // Insert into restr_comp_con
                con.tims_key = ts.tims_key;
                if (SaveComponentAttributeId(con))
                {
                    var owner = GetComponentAttributeId(con, ts, false, connection);
                    if (owner == RestrictionOwner.IsTsOwner) // To get comp_key, an Oracle sequence number
                    {
                        // Create a real vts expression with reference to COMP_KEY just created
                        var expression = string.Format("## = NaN\n!RestrictionExpand:COMP_KEY,{0}\n", con.comp_key);
                        TimeSeriesData.ReplaceExpressionForVTs(con.tims_key, expression);
                    }
                }

                else
                    return RestrictionOwner.CreateOwnerFailed;
            }
            else
                return RestrictionOwner.CreateOwnerFailed;
        }
        else
        {
            con.comp_key = Convert.ToInt32(tbl.Rows[0][0]);
            con.tims_key = Convert.ToInt32(tbl.Rows[0][1]);

            if (Convert.ToInt32(tbl.Rows[0][3]) != 1)
                return RestrictionOwner.IsNotTsOwner; // Time series is shared between models.

            if (con.hydp_key != 0 &&
                (tbl.Rows[0][2] == DBNull.Value || Convert.ToInt32(tbl.Rows[0][2]) != con.hydp_key))
            {
                // Now the parent hyd_key is known - update
                con.comp_key = Convert.ToInt32(tbl.Rows[0][0]);
                UpdateComponentAttributeHydParentKey(con);
            }

            if (ts != null && con.oprt_key == (int) CompAttribute.Avail)
            {
                ts.tims_key = 0;
                ts.tscode = ts.tscode.Replace(".AVL", ".COA");
                var dtCoa = RestrictionData.GetRestrCompConForHydOprtKey(con.hyd_key, (int) CompAttribute.CAvail);
                if (dtCoa.Rows.Count == 0 && !TimeSeriesData.Exists(ts.tscode))
                {
                    TimeSeriesData.CreateTimeSeries(ts,
                        String.Format("## = NaN\n!RestrictionExpand:TIMS_KEY,$<timskey>\n"), connection);
                    var con2 = new RestrCompCon(con.hyd_key, con.hydp_key, (int) CompAttribute.CAvail);
                    con2.comp_key = con.comp_key;
                    con2.tims_key = ts.tims_key;
                    SaveComponentAttributeId(con2);
                }
            }
        }

        return RestrictionOwner.IsTsOwner;
    }

    public static int GetCompKey(int hydKey, int oprtKey)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "select comp_key from restr_comp_con where hyd_key = :1 and oprt_key = :2";
        cmd.Parameters.Add(null, hydKey);
        cmd.Parameters.Add(null, oprtKey);
        object o = Util.CommandToScalar(cmd);
        if (o == null)
            return 0;
        return Convert.ToInt32(o);
    }

    public static DataTable GetCompKey(string hydCompName, int hydtKey, int oprtKey)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText =
            "select a.comp_key,a.hyd_key,b.name,a.oprt_key,a.tscode from view_restr_comp_con a, hydcomp b ";
        cmd.CommandText += "where a.hyd_key=b.hyd_key and b.name = :1 and a.oprt_key = :2 and b.hydt_key = :3";
        cmd.Parameters.Add(null, hydCompName);
        cmd.Parameters.Add(null, oprtKey);
        cmd.Parameters.Add(null, hydtKey);
        return Util.CommandToDataTable(cmd);
    }

    public static DataTable GetCompKey(int timsKey, int hydtKey, int oprtKey)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText =
            "select a.comp_key,a.hyd_key,b.name,a.oprt_key,a.tscode, a.tims_key from view_restr_comp_con a, hydcomp b ";
        cmd.CommandText += "where a.hyd_key=b.hyd_key and a.tims_key = :1 and a.oprt_key = :2 and b.hydt_key = :3";
        cmd.Parameters.Add(null, timsKey);
        cmd.Parameters.Add(null, oprtKey);
        cmd.Parameters.Add(null, hydtKey);
        return Util.CommandToDataTable(cmd);
    }

        public static DataTable GetProgramAccess()
        {
            return Util.SqlToDataTable("select a.*,b.name from application_access a, groups b where a.grup_key=b.grup_key and a.prog_key=80");
        }

        public static void DeleteAllRevisionsForComponent(int compKey)
		{
			OracleCommand oCmd = new OracleCommand();
			oCmd.CommandText = @"
                begin
	                delete from restr_static where oprs_key in (select oprs_key from restrictions where comp_key=:1);
	                delete from restr_attributes where oprs_key in (select oprs_key from restrictions where comp_key=:1);
	                delete from restrictions_tab where comp_key=:1;
                end;";
			oCmd.Parameters.Add(":1", compKey);
			Util.ExecuteCommand(oCmd);
		}

        public static void DeleteCurrentSubscription(int oprsKey)
		{
			OracleCommand oCmd = new OracleCommand();
			oCmd.CommandText = @"
                begin
	                delete from restrictions_tab where oprs_key=:1;
	                delete from restr_static where oprs_key=:1;
	                delete from restr_attributes where oprs_key=:1;
                end;";
			oCmd.Parameters.Add(":1", oprsKey);
			Util.ExecuteCommand(oCmd);
			return;
		}

        public static void DeleteAllSubscriptionForComponent(int compKey)
		{
			OracleCommand oCmd = new OracleCommand();
			oCmd.CommandText = @"
                begin
	                delete from restr_static where oprs_key in (select oprs_key from restrictions where comp_key=:1);
	                delete from restr_attributes where oprs_key in (select oprs_key from restrictions where comp_key=:1);
	                delete from restrictions_tab where comp_key=:1;
                end;";
			oCmd.Parameters.Add(":1", compKey);
			Util.ExecuteCommand(oCmd);
		}

        public static void DeleteComponentAttributeId(int comp_key, int oprtKey, int owner)
		{
			OracleCommand oCmd = new OracleCommand();
			oCmd.CommandText = "delete from restr_comp_con where comp_key=:0 and oprt_key=:1 and owner=:2";
			oCmd.Parameters.Add(null, comp_key);
			oCmd.Parameters.Add(null, oprtKey);
			oCmd.Parameters.Add(null, owner);
			Util.ExecuteCommand(oCmd);
		}

        public static DataTable GetAccessCodeForCompKey(int compKey)
        {
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = ("select acc_code from restrictions where comp_key = :1");
            cmd.Parameters.Add(null, compKey);
            return Util.CommandToDataTable(cmd);
        }

        public static DataTable GetTsForGlobalRef(string globeref, bool useLikeInSql)
        {
            OracleCommand cmd = new OracleCommand();
            if (useLikeInSql)
                cmd.CommandText =
                ("select a.tims_key,a.globeref,b.filename||b.tscode fullname from impexpts a, timeser b where a.tims_key= b.tims_key and globeref like :1"
                );
            else
                cmd.CommandText =
                ("select a.tims_key,a.globeref,b.filename||b.tscode fullname from impexpts a, timeser b where a.tims_key= b.tims_key and  globeref = :1"
                );
            cmd.Parameters.Add(null, globeref);
            return Util.CommandToDataTable(cmd);
        }

    #endregion

    #region FetchMethods

        public static int FetchUserKeyForCurrentUser()
    {
        object o = Util.CommandToScalar(
            new OracleCommand("select ustp_key from users where user_key = pdmisc.fetchUserKey"));
        return Convert.ToInt32(o);
    }

    public static DataTable FetchSubscriptGroupDefinition(int hydKeyh)
    {
        return Util.CommandToDataTable(
            new OracleCommand("select hyd_key from hyrel_ac where hyretype=3 and hyd_keyh=" + hydKeyh));
    }

    public static int FetchStationKeyForGeneratorOrPump(int genKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "select distinct hyd_key,name from hydcomp where hydt_key=6 and ";
        oCmd.CommandText += "hyd_key in (select hyd_key from hyrel_ac where hyd_keyh=:1)";
        oCmd.Parameters.Add(null, genKey);
        return Convert.ToInt32(Util.CommandToScalar(oCmd));
    }

    public static DataTable FetchRestrictionForCompKey(int compKey, int oprsKey, bool getFrequency)
    {
        OracleCommand cmd = new OracleCommand();
        if (oprsKey == 0)
        {
            if (getFrequency)
                cmd.CommandText =
                    "select a.oprs_key,a.vfrom,a.vto,b.s_function,b.value,b.freq_code from restrictions a, restr_static b where a.comp_key = :1 and a.oprs_key= b.oprs_key order by a.vfrom,a.vto";
            else
                cmd.CommandText = "select oprs_key,vfrom,vto from restrictions where comp_key = :1 order by vfrom,vto";
            cmd.Parameters.Add(null, compKey);
        }
        else
        {
            if (getFrequency)
                cmd.CommandText =
                    "select a.oprs_key,a.vfrom,a.vto,b.s_function,b.value,b.freq_code from restrictions a, restr_static b where a.comp_key=:1 and a.oprs_key<>:2 and a.oprs_key=b.oprs_key order by a.vfrom,a.vto";
            else
                cmd.CommandText =
                    "select oprs_key,vfrom,vto from restrictions where comp_key = :1 and oprs_key <> :2 order by vfrom,vto";
            cmd.Parameters.Add(null, compKey);
            cmd.Parameters.Add(null, oprsKey);
        }
        return Util.CommandToDataTable(cmd);
    }

    private static int FetchMaxHydKey()
    {
        object o = Util.CommandToScalar(new OracleCommand("select max(hyd_key) from hydcomp"));
        if (o == DBNull.Value)
            return 0;
        return Convert.ToInt32(o);
    }

    public static int FetchTimsKeyFromRestrCompCon(int avilTimsKey, int compKey)
    {

        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "select tims_key from restr_comp_con where comp_key= :1 and tims_key<> :2";

        oCmd.Parameters.Add(null, compKey);
        oCmd.Parameters.Add(null, avilTimsKey);
        object o = Util.CommandToScalar(oCmd);
        if (o == DBNull.Value)
            return 0;
        return Convert.ToInt32(o);
    }


    public static DataTable FetchRestrDynamicForCompKey(int compKey)
    {
        string sql = "select * from restr_dynamic where oprs_key in ";
        sql += "(select oprs_key from restrictions where comp_key=" + compKey.ToString() + ")";
        return Util.SqlToDataTable(sql);
    }

    public static DataTable FetchHydcoTypes()
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "select hydt_key,code from hydco_tp where hydt_key in (2,4,5,6,7)";
        return Util.CommandToDataTable(oCmd);
    }

    private static int FetchNextOprsKey()
    {
        object o = Util.CommandToScalar(new OracleCommand("select restrictions_seq.nextval from dual"));
        if (o == DBNull.Value)
            return 0;
        return Convert.ToInt32(o);
    }


    public static DataTable FetchNomValues(string sysGroupName, bool includePump)
    {

        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "select a.hyd_key,a.hydt_key,a.hname, b.hyd_key achyd_key, c.attr_id, c.rval ";
        cmd.CommandText += "from view_hydcomp_in_simsysda a, hyrel_ac b, compattr c, hydcomp d ";
        if (includePump)
            cmd.CommandText +=
                "where a.sname = :1 and a.hydt_key in (2,4,5,7,8) and a.hyd_key = b.hyd_keyh and b.hyd_key=d.hyd_key ";
        else
            cmd.CommandText +=
                "where a.sname = :1 and a.hydt_key in (2,4,5,7) and a.hyd_key = b.hyd_keyh and b.hyd_key=d.hyd_key ";
        cmd.CommandText += "and d.hydt_key <> 20 and c.hyd_key = a.hyd_key ";
//			cmd.CommandText += "and c.attr_id in (:2,:3,:4,:5,:6,:7) order by c.attr_id";
        cmd.CommandText += "and c.attr_id in (:2,:3,:4,:5,:6) order by c.attr_id";

        cmd.Parameters.Add(null, sysGroupName);
        cmd.Parameters.Add(null, (int) ATTR_TYPE.RESERVOIR_HRV);
        cmd.Parameters.Add(null, (int) ATTR_TYPE.RESERVOIR_LRV);
        cmd.Parameters.Add(null, (int) ATTR_TYPE.GENERATOR_MAXPROD);
//			cmd.Parameters.Add(null, (int)ATTR_TYPE.GENERATOR_NOMPROD);
        cmd.Parameters.Add(null, (int) ATTR_TYPE.NOMFLOW);
        cmd.Parameters.Add(null, (int) ATTR_TYPE.WTR_MXFLOW);

        return Util.CommandToDataTable(cmd);

    }

    public static string[] FetchSimGroups()
    {
        string sql = "SELECT DISTINCT NAME FROM SIMSYSDA ORDER BY NAME";

        using (DataTable dt = Util.SqlToDataTable(sql))
        {
            string[] array = new string[dt.Rows.Count];

            for (int i = 0; i < dt.Rows.Count; i++)
                array[i] = (string) dt.Rows[i][0];

            return array;
        }
    }

    public static DataTable FetchSelectedSimGroup(string name)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select simsysda.watc_key, simsysda.prd_key, watercou.name wname, prrun_ds.name pname from simsysda, ";
        oCmd.CommandText += "prrun_ds,watercou where watercou.watc_key = simsysda.watc_key and ";
        oCmd.CommandText += "simsysda.prd_key = prrun_ds.prd_key  and (simsysda.name = :pName) order by wname,pname";
        oCmd.Parameters.Add("pName", name);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchRevisionsComponent(int prdKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select distinct a.hyd_key,a.hydt_key,a.hname from view_hydcomp_in_simsysda a, hydcomp b, hyrel_ac c where a. ";
        oCmd.CommandText +=
            "hydt_key in (2,6) and b.hyd_key = c.hyd_keyh and a.hyd_key = c.hyd_key and b.hydt_key in (5,7,8) ";
        oCmd.CommandText += "and a.prd_key=:1 order by 2,3 ";
        oCmd.Parameters.Add(null, prdKey);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchRevisionsAllComponent(int prdKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "select distinct a.hyd_key, a.hydt_key, a.name, c.hyd_key chyd_key, c.hyd_keyh chyd_keyh ";
        oCmd.CommandText += "from hydcomp a, aghcoprr b, hyrel_ac c ";
        oCmd.CommandText +=
            "where a.hyd_key=c.hyd_keyh and a.hydt_key in (2,5,6,7,8) and c.rel_key in (select rel_key from aghcohyr where hyd_key ";
        oCmd.CommandText += " in (select hyd_key from aghcoprr where prd_key = :1)) order by 2,3";
        oCmd.Parameters.Add(null, prdKey);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchRevisions(string sysGroupName)
    {
        OracleCommand oCmd = new OracleCommand();
        //			oCmd.CommandText = "select distinct b.*, a.*, c.*, f.* ";
        //			oCmd.CommandText = "select distinct b.*, ";
        oCmd.CommandText =
            "select distinct b.*, a.hydp_key,a.hyd_key,a.oprt_key,a.name OPRT_NAME, a.tims_key,a.tscode, a.owner, ";
        oCmd.CommandText += "c.prd_key,c.sname,c.hydt_key,c.hname,c.pname,c.wname,c.code, ";
        oCmd.CommandText += "f.s_function,f.value,f.freq_code,f.hours,f.state ";
        oCmd.CommandText += "from  view_restr_comp_con a, restrictions b, view_hydcomp_in_simsysda c, restr_static f ";
        oCmd.CommandText +=
            "where a.owner > -1 and b.oprs_key = f.oprs_key and a.oprt_key = :1 and a.comp_key=b.comp_key ";
        oCmd.CommandText += "and c.hydt_key in (2,4,5,6,7,8) and a.hyd_key = c.hyd_key ";
        if (sysGroupName != string.Empty)
            oCmd.CommandText += "and c.sname=:2 ";
        oCmd.CommandText += "order by c.hname";
        oCmd.Parameters.Add(null, (int) CompAttribute.Avail);
        if (sysGroupName != string.Empty)
            oCmd.Parameters.Add(null, sysGroupName);
        return Util.CommandToDataTable(oCmd);
    }

    public static int FetchAttributeTypeForComponent(int comp_key)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "select oprt_key from restr_comp_con where comp_key=:1";
        oCmd.Parameters.Add(null, comp_key);
        DataTable tbl = Util.CommandToDataTable(oCmd);
        if (tbl.Rows.Count == 1)
            return (int) tbl.Rows[0][0];
        else if (tbl.Rows.Count > 1) // @@ShareBetweenModels - Todo - accept only if all rows have same attribute
            return (int) tbl.Rows[0][0];
        else
            return 0;
    }

    public static bool isCompKeyShareBetweenModels(int hyd_key, int comp_key, int oprt_key)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select hyd_key,oprt_key from restr_comp_con where owner != 0 and hyd_key=:1 and comp_key=:2 and oprt_key=:3";
        oCmd.Parameters.Add(null, comp_key);
        oCmd.Parameters.Add(null, oprt_key);
        oCmd.Parameters.Add(null, hyd_key);
        DataTable tbl = Util.CommandToDataTable(oCmd);
        if (tbl.Rows.Count == 1)
            return false;
        else if (tbl.Rows.Count > 1) // @@ShareBetweenModels - Todo - accept only if all rows have same attribute
            return true;
        else
            return false;
    }

    // --
    public static DataTable FetchTimsKeysForRelatedSeries(int hyd_key, int hydp_key, CompAttribute a)
    {
        OracleCommand cmd = new OracleCommand();
        switch (a)
        {
            case CompAttribute.Avail:
                // Related series are:
                // AVL and UNA series - identified by oprt_key=12000 and oprt_key=12014
                // Contribution from child to parent 
                // Contribution from parent to all children
                if (hydp_key > 0)
                {
                    cmd.CommandText = "select tims_key from restr_comp_con where oprt_key in (12000,12014)";
                    cmd.CommandText += " and hyd_key=:1 union";
                    cmd.CommandText += " select tims_key from restr_comp_con where oprt_key in (12000,12014)";
                    cmd.CommandText += " and hyd_key=:2";
                    cmd.Parameters.Add(null, hyd_key);
                    cmd.Parameters.Add(null, hydp_key);
                }
                else
                {
                    cmd.CommandText = "select tims_key from restr_comp_con where oprt_key in (12000,12014)";
                    cmd.CommandText += " and hyd_key=:1 union";
                    cmd.CommandText += " select tims_key from restr_comp_con where oprt_key in (12000,12014)";
                    cmd.CommandText += " and hydp_key=:1";
                    cmd.Parameters.Add(null, hyd_key);
                }
                break;
            default:
                return new DataTable(); // Empty
        }
        return Util.CommandToDataTable(cmd);
    }

    public static DataTable FetchGeneratorIDs(int hyd_key)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select a.hyd_keyh as hyd_key from hyrel_ac a, hydcomp b where a.hyd_key=:1 and a.hyd_keyh=b.hyd_key and hydt_key=7";
        oCmd.Parameters.Add(null, hyd_key);
        return Util.CommandToDataTable(oCmd);
    }

    /// <summary>
    /// Section for methods for restriction part of the data layer.
    /// </summary>
    /// <param name="sysGroupName"></param>
    /// <param name="rCount"></param>
    /// <returns></returns>
    public static DataTable FetchSubscription(string sysGroupName)
    {
        OracleCommand oCmd = new OracleCommand();
        //			oCmd.CommandText = "select distinct b.*, ";
        oCmd.CommandText =
            "select distinct b.*,a.hydp_key,a.hyd_key,a.oprt_key,a.name OPRT_NAME, a.tims_key,a.tscode,a.owner, ";
        oCmd.CommandText += "c.prd_key,c.sname,c.hydt_key,c.hname,c.pname,c.wname,c.code, ";
        oCmd.CommandText += "f.s_function,f.value ";
        oCmd.CommandText += "from  view_restr_comp_con a, restrictions b, view_hydcomp_in_simsysda c, restr_static f ";
        oCmd.CommandText +=
            "where a.owner > -1 and b.oprs_key = f.oprs_key and a.oprt_key in (:1,:2,:3) and a.comp_key=b.comp_key ";
        oCmd.CommandText += "and c.hydt_key = 6 and a.hyd_key = c.hyd_key and c.sname=:4 ";
        oCmd.CommandText += "order by c.hname";
        oCmd.Parameters.Add(null, (int) CompAttribute.Subscr);
        oCmd.Parameters.Add(null, (int) CompAttribute.SubscrC);
        oCmd.Parameters.Add(null, (int) CompAttribute.SubscrPmi);
        oCmd.Parameters.Add(null, sysGroupName);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchSubscriptionOnGroup(string sysGroupName)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = @"
select distinct b.*,
a.hydp_key,a.hyd_key,a.oprt_key,a.name OPRT_NAME, a.tims_key,a.tscode, a.owner,
s.prd_key,s.sname sname,h.hydt_key,h.name hname,s.pname pname,s.wname wname,htp.code,f.s_function,f.value
from restrictions b
join view_restr_comp_con a on (a.comp_key = b.comp_key and a.oprt_key in (:1,:2,:3))
join restr_static f on (f.oprs_key = b.oprs_key)
join hydcomp h on (h.hydt_key = 23 and h.hyd_key = a.hyd_key)
join hyrel_ac ac on (ac.hyd_keyh = h.hyd_key)
join hydcomp h2 on (h2.hyd_key = ac.hyd_key and h2.hydt_key=6)
join view_hydcomp_in_simsysda s on (s.hyd_key = h2.hyd_key and s.sname=:4)
join hydco_tp htp on (htp.HYDT_KEY = h.hydt_key)
order by h.name";
        //cmd.CommandText = "select distinct b.*, ";
        //cmd.CommandText += "a.hydp_key,a.hyd_key,a.oprt_key,a.name OPRT_NAME, a.tims_key,a.tscode, a.owner, ";
        //cmd.CommandText += "s.prd_key,s.sname sname,h.hydt_key,h.name hname,s.pname pname,s.wname wname,htp.code,f.s_function,f.value ";
        //cmd.CommandText += "from view_restr_comp_con a, restrictions b, view_hydcomp_in_simsysda s, hydcomp h, hydcomp h2, hyrel_ac ac, hydco_tp htp, restr_static f ";
        //cmd.CommandText += "where b.oprs_key = f.oprs_key and a.oprt_key in (:1,:2,:3) and a.comp_key=b.comp_key ";
        //cmd.CommandText += "and h.hydt_key = 23 and (h2.hydt_key=6 and h.hyd_key = ac.hyd_keyh and ac.hyd_key=h2.hyd_key and s.hyd_key=h2.hyd_key) ";
        //cmd.CommandText += "and a.hyd_key = h.hyd_key and h.hydt_key=htp.hydt_key and s.sname=:4 order by h.name ";
        cmd.Parameters.Add(null, (int) CompAttribute.Subscr);
        cmd.Parameters.Add(null, (int) CompAttribute.SubscrC);
        cmd.Parameters.Add(null, (int) CompAttribute.SubscrPmi);
        cmd.Parameters.Add(null, sysGroupName);
        return Util.CommandToDataTable(cmd);
    }

    public static DataTable FetchRestrictionsAdv(string sysGroupName)
    {
        OracleCommand oCmd = new OracleCommand();
        //			oCmd.CommandText = "select distinct b.*, a.*, c.*, f.*, g.tscode ";
        oCmd.CommandText =
            "select distinct b.oprs_key,b.comp_key,b.grup_key,b.group_name,b.user_key,b.dbuser_name,b.osuser_key,b.osuser_name,b.datetim,b.description,b.acc_code, ";
        oCmd.CommandText += "a.hydp_key,a.hyd_key,a.oprt_key,a.name OPRT_NAME, a.tims_key,a.tscode,a.owner, ";
        oCmd.CommandText += "c.prd_key,c.sname,c.hydt_key,c.hname,c.pname,c.wname,c.code, ";
        oCmd.CommandText += "f.priority,f.TIMS_KEY TIMSKEY_PRIORITY, g.tscode TSCODE_PRIORITY, g.curvetyp ";
        oCmd.CommandText +=
            "from  view_restr_comp_con a, restrictions b, view_hydcomp_in_simsysda c, restr_dynamic f, timeser g ";
        oCmd.CommandText +=
            "where a.owner > -1 and b.oprs_key = f.oprs_key and a.oprt_key > :1 and a.comp_key=b.comp_key ";
        oCmd.CommandText +=
            "and c.hydt_key in (2,4,5,6,7,8) and a.hyd_key = c.hyd_key  and c.sname=:2 and f.tims_key = g.tims_key";
        oCmd.Parameters.Add(null, (int) CompAttribute.Avail);
        oCmd.Parameters.Add(null, sysGroupName);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchRestrictions(string sysGroupName)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select distinct b.*,a.hydp_key,a.hyd_key,a.oprt_key,a.name OPRT_NAME, a.tims_key,a.tscode,a.owner, ";
        oCmd.CommandText += "c.prd_key,c.sname,c.hydt_key,c.hname,c.pname,c.wname,c.code, ";
        oCmd.CommandText += "f.s_function,f.value,f.freq_code,f.hours,f.state ";
        oCmd.CommandText += "from  view_restr_comp_con a, restrictions b, view_hydcomp_in_simsysda c, restr_static f ";
        oCmd.CommandText +=
            "where a.owner > -1 and b.oprs_key = f.oprs_key and (a.oprt_key > :1 and a.oprt_key < :2 or a.oprt_key > :3 and a.oprt_key <> :4) ";
        oCmd.CommandText += "and a.comp_key=b.comp_key ";
        oCmd.CommandText +=
            "and c.hydt_key in (2,4,5,6,7,8) and a.hyd_key = c.hyd_key  and c.sname=:5 order by c.hname";
        oCmd.Parameters.Add(null, (int) CompAttribute.Avail);
        oCmd.Parameters.Add(null, (int) CompAttribute.Subscr);
        oCmd.Parameters.Add(null, (int) CompAttribute.SubscrPmi);
        oCmd.Parameters.Add(null, (int) CompAttribute.CAvail);
        oCmd.Parameters.Add(null, sysGroupName);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchPowerPlantComponent(int prdKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "select distinct a.hyd_key,a.hydt_key,a.hname from view_hydcomp_in_simsysda a where ";
        oCmd.CommandText += "a.hydt_key = 6 and a.prd_key=:1 order by 2,3";
        oCmd.Parameters.Add(null, prdKey);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchSubscriptGroupComponent(int prdKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = @"
select distinct hydcomp.hyd_key,hydcomp.hydt_key,hydcomp.name 
from hydcomp, hyrel_ac ac, view_hydcomp_in_sim_model simModel 
where simModel.prd_key = :1 and simModel.hydt_key = 6 and simModel.hyd_key = ac.hyd_key and
ac.hyd_keyh = hydcomp.hyd_key and hydcomp.hydt_key=23";
        oCmd.Parameters.Add(null, prdKey);
        return Util.CommandToDataTable(oCmd);
    }


    //TODO sjekk linjeskift
    //TODO kommentarer!!!
    public static DataTable FetchRestrictionsForGatesComponent(List<int> prdKeys)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select distinct view_hydcomp_in_simsysda.hyd_key,view_hydcomp_in_simsysda.hydt_key,view_hydcomp_in_simsysda.hname || ' (' || view_hydcomp_in_simsysda.wname || '.' || view_hydcomp_in_simsysda.pname || ')' hname ";
        oCmd.CommandText += "from view_hydcomp_in_simsysda, hydcomp, hyrel_ac ";
        oCmd.CommandText +=
            "where view_hydcomp_in_simsysda.hydt_key = 2 and hydcomp.hyd_key = hyrel_ac.hyd_keyh and view_hydcomp_in_simsysda.hyd_key = hyrel_ac.hyd_key and hydcomp.hydt_key = 5 ";
        oCmd.CommandText += "and view_hydcomp_in_simsysda.prd_key in(";

        bool first = true;
        foreach (int key in prdKeys)
        {

            if (first)
            {
                oCmd.CommandText += key;
                first = false;
            }
            else oCmd.CommandText += "," + key;
        }


        oCmd.CommandText += " ) order by view_hydcomp_in_simsysda.hydt_key";

        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchRestrictionsGateComponent(int prdKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select distinct a.hyd_key,a.hydt_key,a.hname from view_hydcomp_in_simsysda a, hydcomp b, hyrel_ac c where a. ";
        oCmd.CommandText += "hydt_key = 2 and b.hyd_key = c.hyd_keyh and a.hyd_key = c.hyd_key and b.hydt_key = 5 ";
        oCmd.CommandText += "and a.prd_key=:1 order by 2,3 ";
        oCmd.Parameters.Add(null, prdKey);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchRestrictionsComponent(int prdKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "select distinct a.hyd_key,a.hydt_key,a.hname from view_hydcomp_in_simsysda a where ";
        oCmd.CommandText += "a.hydt_key in (2,4,5,6,7) and a.prd_key=:1 order by 2,3";
        oCmd.Parameters.Add(null, prdKey);
        return Util.CommandToDataTable(oCmd);
    }

    /// <summary>
    /// Fetch restrictions based on given prdKeys
    /// </summary>
    /// <param name="prdKeys">List of prdKeys</param>

    public static DataTable FetchRestrictionsComponent(List<int> prdKeys)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "select distinct hyd_key,hydt_key,hname || ' (' || wname || '.' || pname || ')' hname ";
        oCmd.CommandText += "from view_hydcomp_in_simsysda where ";
        oCmd.CommandText += "hydt_key in (2,4,5,6,7) and prd_key in(";

        bool first = true;
        foreach (int i in prdKeys)
        {
            if (first)
            {
                oCmd.CommandText += i;
                first = false;
            }
            else oCmd.CommandText += "," + i;
        }


        oCmd.CommandText += " ) order by hydt_key";
        return Util.CommandToDataTable(oCmd);
    }




    public static DataTable FetchObjectForSystemGroup(string groupName)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select distinct a.hyd_key,a.hydt_key,a.hname,a.wname from view_hydcomp_in_simsysda a, hyrel_ac c where ";
        oCmd.CommandText += "a.hydt_key in (2,4,5,6,7) and a.hyd_key = c.hyd_keyh ";
        oCmd.CommandText += "and a.sname = :1 order by 2,3";
        oCmd.Parameters.Add(null, groupName);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchObjectForSystemGroup(string groupName, int hydtKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select distinct a.hyd_key,a.hydt_key,a.hname,a.wname,a.prd_key,a.pname from view_hydcomp_in_simsysda a where ";
        oCmd.CommandText += "a.hydt_key = :1 and a.sname = :2 order by 2,3";
        oCmd.Parameters.Add(null, hydtKey);
        oCmd.Parameters.Add(null, groupName);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchRestrictionsAllComponent(int prdKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select distinct a.hyd_key, a.hydt_key, a.name, c.hyd_key chyd_key, c.hyd_keyh chyd_keyh from hydcomp a, aghcoprr b, hyrel_ac c ";
        oCmd.CommandText +=
            "where a.hyd_key=c.hyd_keyh and a.hydt_key in (2,4,5,6,7) and c.rel_key in (select rel_key from aghcohyr where hyd_key ";
        oCmd.CommandText += " in (select hyd_key from aghcoprr where prd_key = :1)) order by 2,3";
        oCmd.Parameters.Add(null, prdKey);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchRestrictionsGateAndWatercouComponent(int prdKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "select distinct a.hyd_key, a.hydt_key, a.name, c.hyd_key chyd_key, c.hyd_keyh chyd_keyh ";
        oCmd.CommandText += "from hydcomp a, aghcoprr b, hyrel_ac c, hydcomp e ";
        oCmd.CommandText +=
            "where a.hyd_key=c.hyd_keyh and e.hyd_key=c.hyd_key and e.hydt_key <> 20 and a.hydt_key in (2,5) and c.rel_key ";
        oCmd.CommandText += "in (select rel_key from aghcohyr where hyd_key ";
        oCmd.CommandText += "in (select hyd_key from aghcoprr where prd_key = :1)) order by 2,3";
        oCmd.Parameters.Add(null, prdKey);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchRestrictionsPlantComponent(int prdKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "select distinct a.hyd_key, a.hydt_key, a.name from hydcomp a, aghcoprr b, hyrel_ac c ";
        oCmd.CommandText +=
            "where a.hyd_key=c.hyd_keyh and a.hydt_key = 6 and c.rel_key in (select rel_key from aghcohyr where hyd_key ";
        oCmd.CommandText += " in (select hyd_key from aghcoprr where prd_key = :1)) order by 2,3";
        oCmd.Parameters.Add(null, prdKey);
        return Util.CommandToDataTable(oCmd);
    }

    public static DataTable FetchRestrictionAttributeDefinitions()
    {
        return Util.SqlToDataTable("select * from restr_attributes_tp order by attr_key,attrp_key");
    }

    public static DataTable FetchRestrictionAttributeValues(int OprtKey)
    {
        OracleCommand cmd = new OracleCommand(
            @"select attr_key, cvalue from restr_attributes where oprs_key = :1");
        cmd.Parameters.Add(null, OprtKey);

        return Util.CommandToDataTable(cmd);
    }

    private static int FetchMaxRelKey()
    {
        object o = Util.CommandToScalar(new OracleCommand("select max(rel_key) from hyrel_ac"));
        if (o == DBNull.Value)
            return 0;
        return Convert.ToInt32(o);
    }

    public static DataTable FetchComponentAndTimsKeyFromSystemGroup(String sysGroup)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "select distinct a.prd_key,a.hyd_key,a.hydt_key,a.HNAME,b.tims_key,b.tscode,b.oprt_key ";
        cmd.CommandText += "from view_hydcomp_in_simsysda a, view_restr_comp_con b ";
        cmd.CommandText += "where a.sname=:1 and a.hyd_key = b.hyd_key and b.oprt_key in (12000,12001,12005) ";
        cmd.CommandText += "order by hyd_key,oprt_key";
        cmd.Parameters.Add(null, sysGroup);
        return Util.CommandToDataTable(cmd);
    }

    public static DataTable FetchComponentAndTimsKeyFromDataSet(int prdKey, ArrayList oprtList, bool isMinMax)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "select distinct a.prd_key,a.hyd_key,a.hydt_key,a.HNAME,b.tims_key,b.tscode,b.oprt_key ";
        cmd.CommandText += "from view_hydcomp_in_simsysda a, view_restr_comp_con b ";
        cmd.CommandText += "where a.prd_key=:1 and a.hyd_key = b.hyd_key and b.oprt_key in (";
        for (int i = 0; i < oprtList.Count; i++)
        {
            if (i == 0) cmd.CommandText += oprtList[i].ToString();
            else cmd.CommandText += "," + oprtList[i].ToString();
        }
        cmd.CommandText += ") order by ";
        if (isMinMax)
            cmd.CommandText += "hyd_key,oprt_key";
        else cmd.CommandText += "oprt_key DESC,hyd_key";
        cmd.Parameters.Add(null, prdKey);
        return Util.CommandToDataTable(cmd);
    }

    /// <summary>
    /// Fetch timskeys and other data based on given hyd_keys and oprt_key
    /// </summary>
    /// <param name="hydKeys">List of hydKeys</param>
    /// <param name="oprtList">List of oprtKeys</param>
    public static DataTable FetchTimsKeyFromComponent(List<int> hydKeys, List<int> oprtList)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText =
            "select distinct view_hydcomp_in_simsysda.prd_key,view_hydcomp_in_simsysda.hyd_key,view_hydcomp_in_simsysda.hydt_key,view_hydcomp_in_simsysda.HNAME,view_restr_comp_con.tims_key,view_restr_comp_con.tscode,view_restr_comp_con.oprt_key ";
        cmd.CommandText += "from view_hydcomp_in_simsysda, view_restr_comp_con ";
        cmd.CommandText += "where view_hydcomp_in_simsysda.hyd_key in (";
        bool first = true;
        foreach (int i in hydKeys)
        {
            if (first)
            {
                cmd.CommandText += i;
                first = false;
            }
            else cmd.CommandText += "," + i;
        }
        cmd.CommandText +=
            ") and view_hydcomp_in_simsysda.hyd_key = view_restr_comp_con.hyd_key and view_restr_comp_con.oprt_key in (";
        first = true;
        foreach (int i in oprtList)
        {
            if (first)
            {
                cmd.CommandText += i;
                first = false;
            }
            else cmd.CommandText += "," + i;
        }
        cmd.CommandText += ") order by oprt_key";

        return Util.CommandToDataTable(cmd);
    }


    public static DataTable FetchAllComponents(int prdKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select distinct a.hyd_key, a.hydt_key, a.name, c.hyd_key chyd_key, c.hyd_keyh chyd_keyh from hydcomp a, aghcoprr b, hyrel_ac c ";
        oCmd.CommandText +=
            "where a.hyd_key=c.hyd_keyh and a.hydt_key in (2,4,5,6,7) and c.rel_key in (select rel_key from aghcohyr where hyd_key ";
        oCmd.CommandText += " in (select hyd_key from aghcoprr where prd_key = :1)) order by 2,3";
        oCmd.Parameters.Add(null, prdKey);
        return Util.CommandToDataTable(oCmd);
    }

    /// <summary>
    /// Fetch component data based on prdKeys
    /// </summary>
    /// <param name="prdKeys">List of prdKeys</param>
    public static DataTable FetchAllComponents(List<int> prdKeys)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "select distinct hydcomp.hyd_key, hydcomp.hydt_key, hydcomp.name name, hyrel_ac.hyd_key chyd_key, hyrel_ac.hyd_keyh chyd_keyh, view_restr_comp_con.tims_key,view_restr_comp_con.tscode ";
        oCmd.CommandText += "from hydcomp, view_restr_comp_con, hyrel_ac ";
        oCmd.CommandText +=
            "where hydcomp.hyd_key=hyrel_ac.hyd_keyh and hydcomp.hydt_key in (2,4,5,6,7) and hyrel_ac.rel_key in (select rel_key from aghcohyr where hyd_key ";
        oCmd.CommandText += " in (select hyd_key from aghcoprr where prd_key in(";
        bool first = true;
        foreach (int i in prdKeys)
        {
            if (first)
            {
                oCmd.CommandText += i;
                first = false;
            }
            else oCmd.CommandText += "," + i;
        }

        oCmd.CommandText += " ))) and hydcomp.hyd_key=view_restr_comp_con.hyd_key and view_restr_comp_con.oprt_key in";
        oCmd.CommandText += " (" + (int) CompAttribute.Priority + "," + (int) CompAttribute.Weight +
                            ") order by hydcomp.hydt_key, hydcomp.name";


        return Util.CommandToDataTable(oCmd);
    }



    public static DataTable FetchCrossReferenceForTimsKey(int timsKey)
    {
        /************************************************************************/
        /* Get cross reference for current tims_key in spiderx table. 
         * Returning data table with this infomation                            */
        /************************************************************************/
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText =
            "select OBJ_KEY,OBJ_KEB,XREFTYPE,DESCRIPT,DATATYPE,DATATYPE_ICC,NAME from SPIDERX where OBJ_KEY = :1";
        cmd.Parameters.Add(null, timsKey);
        return Util.CommandToDataTable(cmd);
    }

    public static DataRow FetchAttribute(int hydKey, int attributeID)
    {
        OracleCommand cmd = new OracleCommand("select * from compattr where hyd_key=:1 and attr_id=:2");
        cmd.Parameters.Add(null, hydKey);
        cmd.Parameters.Add(null, attributeID);

        DataTable dt = Util.CommandToDataTable(cmd);

        if (dt.Rows.Count > 0)
            return dt.Rows[0];
        else
            return null;
    }

    public static DataTable FetchRestrictions(string revisionCodes, string systemGroup)
    {
        string codes = "10200,10201,10202,10207,10208,10215,10216,10217,10219";

        if (revisionCodes != null && revisionCodes.Length > 0)
            codes = revisionCodes;

        StringBuilder query = new StringBuilder();
        query.Append(
            "SELECT PRRUN_DS.PRD_KEY, WATERCOU.WATC_KEY,WATERCOU.NAME || '.' || PRRUN_DS.NAME SYSNAME,OPRRST.OPRT_KEY,OPRRST.HYD_KEY,OPRCOFUN.FUN_KEY,");
        query.Append("OPRRST.OPRS_KEY, OPRRS_VL.SEQNO,OPRRS_VL.STARTIME,OPRRS_VL.FINTIME,OPRRS_VL.APPROVAL_DEADLINE,");
        query.Append("OPRRS_VL.APPROVED_BY,OPRRS_VL.JOB_TYPE,OPRRS_VL.DESCRIPT,OPRRS_VL.REST_VL1,OPRRS_VL.REST_VL2,");
        query.Append(
            "OPRRS_VL.STARTEND,OPRRS_VL.JOB_NUMBER,OPRRS_VL.JOB_DESCRIPTION,OPRRS_VL.NUMBER_OF_HOURS,OPRRST.UNME_KEY,");
        query.Append(
            "OPRRS_VL.LAST_MODIFIED,OPRRS_VL.MODIFIED_BY,OPRRS_VL.CONTACT_NAME,OPRRS_VL.DATE_APPROVED_BY_STATNETT,");
        query.Append("OPRRS_VL.EXTERNAL_DEPENDENCIES,OPRRS_VL.CONCTRACT_SIGNED,OPRRS_VL.ACTUAL_START_DATE,");
        query.Append("OPRRS_VL.ACTUAL_END_DATE,OPRRS_VL.CREATED, HYDCOMP.HYDT_KEY, HYDCOMP.NAME OBJECT_NAME");
        query.Append(" FROM OPRCOFUN,OPRRST,PRRUN_DS,OPRRS_VL, WATERCOU, HYDCOMP ");
        query.Append(" WHERE PRRUN_DS.WATC_KEY = WATERCOU.WATC_KEY");
        query.Append(" AND OPRCOFUN.OPRS_KEY = OPRRST.OPRS_KEY");
        query.Append(" AND PRRUN_DS.FUN_KEY = OPRCOFUN.FUN_KEY");
        query.Append(" AND OPRRST.OPRS_KEY = OPRRS_VL.OPRS_KEY");
        query.Append(" AND OPRRST.HYD_KEY = HYDCOMP.HYD_KEY");
        query.Append(" AND (OPRRST.OPRT_KEY in (" + codes + "))");
        query.Append(" AND (PRRUN_DS.PRD_KEY in ");
        if (systemGroup.Length > 0)
            query.Append(" (select PRD_KEY from SIMSYSDA where NAME = '" + systemGroup + "'))");
        else
            query.Append(" (select distinct PRD_KEY from SIMSYSDA))");
        query.Append(" order by prd_key,hyd_key,oprt_key,oprs_key,seqno");

        DataTable restriction = Util.SqlToDataTable(query.ToString());
        restriction.TableName = "restriction";
        return restriction;


    }


    public static DataTable FetchAttributes(int[] attributeIDs)
    {
        StringBuilder sql = new StringBuilder();
        sql.Append("select * from compattr where attr_id in (");

        for (int i = 0; i < attributeIDs.Length; i++)
        {
            if (i > 0)
                sql.Append(", ");

            sql.Append(attributeIDs[i]);
        }

        sql.Append(")");

        return Util.SqlToDataTable(sql.ToString());
    }

    #endregion

    #region SaveMethods

    public static bool SaveConsumptionTimeSeries(int hydKey, int timsKey, ATTR_TYPE attrType)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "select distinct ival from compattr where seq_no=0 and hyd_key=:1 and attr_id=:2";
        cmd.Parameters.Add(null, hydKey);
        cmd.Parameters.Add(null, (int) attrType);
        DataTable dt = Util.CommandToDataTable(cmd);
        cmd.Parameters.Clear();
        if (dt.Rows.Count > 0)
        {
            if ((int) dt.Rows[0]["ival"] != timsKey)
            {
                cmd.CommandText = "update compattr set ival = :1 where hyd_key=:2 and attr_id=:3";
                cmd.Parameters.Add(null, timsKey);
                cmd.Parameters.Add(null, hydKey);
                cmd.Parameters.Add(null, (int) attrType);
                Util.ExecuteCommand(cmd);
            }
            else return false;
        }
        else
        {
            cmd.CommandText = "insert into compattr(hyd_key,attr_id,seq_no,ival) values(:1,:2,0,:3)";
            cmd.Parameters.Add(null, hydKey);
            cmd.Parameters.Add(null, (int) attrType);
            cmd.Parameters.Add(null, timsKey);
            Util.ExecuteCommand(cmd);
        }
        return true;
    }

    public static int SaveIntoRestrictions(int compKey, DateTime vfrom, DateTime vto, string description)
    {
        int oprsKey = -1;
        oprsKey = FetchNextOprsKey();
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "insert into restrictions_tab (oprs_key, comp_key, vfrom, vto, description) values (:1, :2, :3, :4, :5)";
        oCmd.Parameters.Add(null, oprsKey);
        oCmd.Parameters.Add(null, compKey);
        oCmd.Parameters.Add(null, vfrom);
        oCmd.Parameters.Add(null, vto);
        oCmd.Parameters.Add(null, description);
        Util.ExecuteCommand(oCmd);
        return oprsKey;
    }

    public static int SaveIntoRestrictions(int compKey, DateTime vfrom, DateTime vto, string description,
        string externalId, string externalSource, string externalId2, string externalSource2)
    {
        int oprsKey = -1;
        oprsKey = FetchNextOprsKey();
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "insert into restrictions_tab (oprs_key, comp_key, vfrom, vto, description,external_id,external_source,external_id2,external_source2) values (:1, :2, :3, :4, :5, :6, :7, :8, :9)";
        oCmd.Parameters.Add(null, oprsKey);
        oCmd.Parameters.Add(null, compKey);
        oCmd.Parameters.Add(null, vfrom);
        oCmd.Parameters.Add(null, vto);
        oCmd.Parameters.Add(null, description);
        oCmd.Parameters.Add(null, externalId);
        oCmd.Parameters.Add(null, externalSource);
        oCmd.Parameters.Add(null, externalId2);
        oCmd.Parameters.Add(null, externalSource2);
        Util.ExecuteCommand(oCmd);
        return oprsKey;
    }

    public static int SaveIntoRestrictions(int compKey, DateTime vfrom, DateTime vto, string description,
        string externalId, string externalSource)
    {
        int oprsKey = -1;
        oprsKey = FetchNextOprsKey();
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText =
            "insert into restrictions_tab (oprs_key, comp_key, vfrom, vto, description,external_id,external_source) values (:1, :2, :3, :4, :5, :6, :7)";
        oCmd.Parameters.Add(null, oprsKey);
        oCmd.Parameters.Add(null, compKey);
        oCmd.Parameters.Add(null, vfrom);
        oCmd.Parameters.Add(null, vto);
        oCmd.Parameters.Add(null, description);
        oCmd.Parameters.Add(null, externalId);
        oCmd.Parameters.Add(null, externalSource);
        Util.ExecuteCommand(oCmd);
        return oprsKey;

    }

    public static void SaveIntoSimsysda(string groupName, int watcKey, int prdKey)
    {
        object o = Util.CommandToScalar(new OracleCommand("select max(sysd_key) from simsysda"));
        int sysdKey = 1;
        if (o != DBNull.Value)
        {
            sysdKey = Convert.ToInt32(o);
            sysdKey = sysdKey + 1;
        }
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "insert into simsysda(sysd_key, name, watc_key, prd_key) values (";
        oCmd.CommandText += ":1, :2, :3, :4)";
        oCmd.Parameters.Add(null, sysdKey);
        oCmd.Parameters.Add(null, groupName);
        oCmd.Parameters.Add(null, watcKey);
        oCmd.Parameters.Add(null, prdKey);
        Util.ExecuteCommand(oCmd);
    }

    public static void SaveIntoRestrStatic(int oprsKey, short sFunc, double val, short freqKey, short hours)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "insert into restr_static(oprs_key, s_function, value, freq_code, hours) values (";
        oCmd.CommandText += ":1, :2, :3, :4, :5)";
        oCmd.Parameters.Add(null, oprsKey);
        oCmd.Parameters.Add(null, sFunc);
        oCmd.Parameters.Add(null, val);
        oCmd.Parameters.Add(null, freqKey);
        oCmd.Parameters.Add(null, hours);
        Util.ExecuteCommand(oCmd);
    }

    public static void SaveIntoRestrStatic(int oprsKey, short sFunc, double val, short freqKey, short hours,
        short state)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "insert into restr_static(oprs_key, s_function, value, freq_code, hours, state) values (";
        oCmd.CommandText += ":1, :2, :3, :4, :5, :6)";
        oCmd.Parameters.Add(null, oprsKey);
        oCmd.Parameters.Add(null, sFunc);
        oCmd.Parameters.Add(null, val);
        oCmd.Parameters.Add(null, freqKey);
        oCmd.Parameters.Add(null, hours);
        oCmd.Parameters.Add(null, state);
        Util.ExecuteCommand(oCmd);
    }

    public static void SavePlantConToSubScriptGroup(int hydKey, DataTable dt)
    {
        int i = 1;
        foreach (DataRow dr in dt.Rows)
        {
            int hydKeyh = (int) dr[0];
            OracleCommand cmd = new OracleCommand();
            cmd.CommandText = "insert into hydcomp(HYD_KEY,HYDT_KEY,VERSION,HYD_KEYH) values(:1,:2,:3,:4)";
            cmd.Parameters.Add(null, hydKey);
            cmd.Parameters.Add(null, 10);
            cmd.Parameters.Add(null, i);
            cmd.Parameters.Add(null, hydKeyh);
            Util.ExecuteCommand(cmd);
            i = i + 1;
        }
    }

    public static bool UpdateComponentAttributeId(RestrictionData.RestrCompCon con)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText =
            "select distinct tims_key from restr_comp_con where hydp_key=:1 and hyd_key=:2 and oprt_key=:3 and comp_key=:4 and owner=:5";
        cmd.Parameters.Add(null, con.hydp_key);
        cmd.Parameters.Add(null, con.hyd_key);
        cmd.Parameters.Add(null, con.oprt_key);
        cmd.Parameters.Add(null, con.comp_key);
        cmd.Parameters.Add(null, con.owner);
        DataTable dt = Util.CommandToDataTable(cmd);
        if (dt.Rows.Count < 1) return false;
        int timsKey = (int) dt.Rows[0][0];
        if (timsKey < 1) return false;
        cmd.Parameters.Clear();
        cmd.CommandText = "update restr_comp_con set tims_key = :1 where tims_key = :2";
        cmd.Parameters.Add(null, con.tims_key);
        cmd.Parameters.Add(null, timsKey);
        try
        {
            Util.ExecuteCommand(cmd);
        }
        catch (Exception e)
        {
            return false;
        }
        return true;
    }

    public static bool SaveComponentAttributeId(RestrictionData.RestrCompCon con)
    {
        if (con.comp_key == 0)
            con.comp_key = FetchNextCompKey();
        OracleCommand oICmd = new OracleCommand();
        oICmd.CommandText = "insert into restr_comp_con ";

        if (con.hydp_key > 0)
        {
            oICmd.CommandText +=
                "(hydp_key,hyd_key,oprt_key,tims_key,comp_key,owner) values (:p1, :p2, :p3, :p4, :p5, :p6)";
            oICmd.Parameters.Add(null, con.hydp_key); // p1
        }
        else
            oICmd.CommandText += "(hyd_key,oprt_key,tims_key,comp_key,owner) values (:p2, :p3, :p4, :p5, :p6)";
        oICmd.Parameters.Add(null, con.hyd_key);
        oICmd.Parameters.Add(null, con.oprt_key);
        oICmd.Parameters.Add(null, con.tims_key);
        oICmd.Parameters.Add(null, con.comp_key);
        oICmd.Parameters.Add(null, con.owner);
        try
        {
            Util.ExecuteCommand(oICmd);
        }
        catch (Exception e)
        {
            // Row allready exists, (old time series deleted manually) try update with new tims_key
            if (Environment.GetEnvironmentVariable("ICC_RESTR_DELETE_ALL_MENU") != null)
            {
                if (!UpdateComponentAttributeId(con))
                {
                    Log.Error($"\n\tException caught:\n{e}");
                }
            }
            else
            {
                    Log.Error($"\n\tException caught:\n{e}");
            }
        }
        // Get comp_key
        string sql = String.Format("select comp_key from restr_comp_con where hyd_key={0} and oprt_key={1}",
            con.hyd_key, con.oprt_key);
        Object o = Util.SqlToScalar(sql);
        if (o == null) return false;
        else con.comp_key = Convert.ToInt32(o);
        return true;
    }

    private static int FetchNextCompKey()
    {
        object o = Util.CommandToScalar(new OracleCommand("select restr_comp_con_seq.nextval from mydual"));
        if (o == DBNull.Value)
            return 0;
        return Convert.ToInt32(o);
    }

    public static int SaveIntoRestrictionsAdvance(int compKey, string description)
    {
        int oprsKey = -1;
        oprsKey = FetchNextOprsKey();
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "insert into restrictions_tab (oprs_key, comp_key, description) values (:1, :2, :3)";
        oCmd.Parameters.Add(null, oprsKey);
        oCmd.Parameters.Add(null, compKey);
        oCmd.Parameters.Add(null, description);
        Util.ExecuteCommand(oCmd);
        return oprsKey;
    }

    public static void SaveIntoRestrDynamic(int oprsKey, short priority, int timsKey)
    {
        OracleCommand oCmd = new OracleCommand();
        oCmd.CommandText = "insert into restr_dynamic(oprs_key, priority, tims_key) values (";
        oCmd.CommandText += ":1, :2, :3)";
        oCmd.Parameters.Add(null, oprsKey);
        oCmd.Parameters.Add(null, priority);
        oCmd.Parameters.Add(null, timsKey);
        Util.ExecuteCommand(oCmd);
    }

    public static void SaveRestrictionAttributeValues(int OprtKey, int attributeKey, string value)
    {
        OracleCommand cmd = new OracleCommand(
            @"declare n number;
begin
select count(*) into n from restr_attributes where oprs_key = :oprs_key and attr_key = :attr_key;
if (n = 0) then
	insert into restr_attributes (oprs_key, attr_key, cvalue) values (:oprs_key, :attr_key, :cvalue);
else
	update restr_attributes set cvalue = :cvalue where oprs_key = :oprs_key and attr_key = :attr_key;
end if;
end;");
        cmd.BindByName = true;
        cmd.Parameters.Add(":oprs_key", OprtKey);
        cmd.Parameters.Add(":attr_key", attributeKey);
        cmd.Parameters.Add(":cvalue", value);

        Util.ExecuteCommand(cmd);
    }

    public static void SaveAttribute(int hydKey, int attributeID, bool value)
    {
        SaveAttribute(hydKey, attributeID, Convert.ToInt32(value));
    }

    public static void SaveAttribute(int hydKey, int attributeID, int value)
    {
        DataRow dr = FetchAttribute(hydKey, attributeID);

        if (dr == null)
            InsertAttribute(hydKey, attributeID, value);
        else if ((int) dr["ival"] != value)
            UpdateAttribute(hydKey, attributeID, value);
    }

    static void InsertAttribute(int hydKey, int attributeID, int value)
    {
        OracleCommand cmd =
            new OracleCommand("insert into compattr (hyd_key, attr_id, seq_no, ival) values (:1, :2, 0, :3)");
        cmd.Parameters.Add(null, hydKey);
        cmd.Parameters.Add(null, attributeID);
        cmd.Parameters.Add(null, value);

        Util.ExecuteCommand(cmd);
    }

    static void UpdateAttribute(int hydKey, int attributeID, int value)
    {
        OracleCommand cmd = new OracleCommand("update compattr set ival = :1 where hyd_key = :2 and attr_id = :3");
        cmd.Parameters.Add(null, value);
        cmd.Parameters.Add(null, hydKey);
        cmd.Parameters.Add(null, attributeID);

        Util.ExecuteCommand(cmd);
    }

    #endregion

    #region IsMethods

    public static int isRestrictionDefined(int hydKey, int oprtKey, short priKey)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "select c.oprs_key from restr_comp_con a,restrictions b, restr_dynamic c ";
        cmd.CommandText +=
            "where a.comp_key = b.comp_key and b.oprs_key = c.oprs_key and a.hyd_key = :1 and a.oprt_key = :2 and c.priority = :3";
        cmd.Parameters.Add(null, hydKey);
        cmd.Parameters.Add(null, oprtKey);
        cmd.Parameters.Add(null, priKey);
        DataTable dt = Util.CommandToDataTable(cmd);
        if (dt.Rows.Count != 0)
            return (int) dt.Rows[0][0];
        return -1;
    }

    #endregion

    #region CheckMethods

    public static bool CheckSubscriptionGroupInUse(int hydKey)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "Select oprs_key from restrictions where comp_key in ";
        cmd.CommandText += "(select comp_key from restr_comp_con where oprt_key in (12011,12013) and hyd_key = :1)";
        cmd.Parameters.Add(null, hydKey);
        DataTable dt = Util.CommandToDataTable(cmd);
        if (dt.Rows.Count > 0)
            return true;
        return false;
    }

    public static int CheckHydcompName(string hydName)
    {
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "select hyd_key,name from hydcomp where upper(name) = upper(:1)";
        cmd.Parameters.Add(null, hydName);
        DataTable dt = Util.CommandToDataTable(cmd);
        if (dt.Rows.Count == 0) return -1;
        else return Convert.ToInt32(dt.Rows[0][0]);
    }

    #endregion

    #region CreateMethods

    public static bool CreateNewHyrelAcConnection(int hydKeyh, int hydKey)
    {
        int relKey = FetchMaxRelKey();
        if (relKey == 0) return false;
        relKey = relKey + 1;
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "insert into hyrel_ac(REL_KEY,HYRETYPE,HYD_KEYH,HYD_KEY) values(:1,3,:2,:3)";
        cmd.Parameters.Add(null, relKey);
        cmd.Parameters.Add(null, hydKeyh);
        cmd.Parameters.Add(null, hydKey);
        Util.ExecuteCommand(cmd);
        return true;
    }

    public static int CreateNewHydComp(int hydtKey, int watchKey, string hydName, out bool inUse)
    {
        int maxHydkey = FetchMaxHydKey();
        maxHydkey = maxHydkey + 1;
        DataTable dt = Util.CommandToDataTable(new OracleCommand(
            "select hyd_key from hydcomp where upper(name)='" + hydName.ToUpper() + "' and hydt_key=" + hydtKey));
        if (dt.Rows.Count > 0)
        {
            inUse = true;
            return (int) dt.Rows[0][0];
        }
        else inUse = false;
        OracleCommand cmd = new OracleCommand();
        cmd.CommandText = "insert into hydcomp(HYD_KEY,WATC_KEY,HYDT_KEY,NAME,VERSION) values(:1,:2,:3,:4,0)";
        cmd.Parameters.Add(null, maxHydkey);
        cmd.Parameters.Add(null, watchKey);
        cmd.Parameters.Add(null, hydtKey);
        cmd.Parameters.Add(null, hydName);
        Util.ExecuteCommand(cmd);
        return maxHydkey;
    }

    #endregion

    #region CountMethods

    public static int CountRestrCompCon()
    {
        return Convert.ToInt32(Util.SqlToScalar("select count(*) from restr_comp_con"));
    }

    public static int CountRestrictions(int oprsKey)
    {
        return Convert.ToInt32(
            Util.SqlToScalar("select count(*) from restrictions where oprs_key=" + oprsKey.ToString()));
    }

    #endregion

    #region InsertMethods

    public static int InsertRestrAttributes(int attrPkey, String name, String type, String description)
    {
        int attrKey = -1;
        OracleCommand cmd = new OracleCommand();
        Object o = Util.SqlToScalar("select max(attr_key) from tp_restr_attributes_tp");
        Object o3 = Util.SqlToScalar("select max(attr_key) from restr_attributes");
        if (o == DBNull.Value) o = 100;
        if (o != DBNull.Value)
        {
            attrKey = Convert.ToInt32(o);
            if (o3 != DBNull.Value)
            {
                if (attrKey < Convert.ToInt32(o3))
                    attrKey = Convert.ToInt32(o3);
            }
            attrKey = attrKey + 1;
            DataTable dtLangKeys = Util.SqlToDataTable("select distinct language_key from icc_language");
            foreach (DataRow r in dtLangKeys.Rows)
            {
                cmd.CommandText =
                    "insert into tp_restr_attributes_tp(ATTR_KEY,LANGUAGE_KEY,ATTRP_KEY,INUSE,NAME,TYPE,DESCRIPTION,SEQNO) ";
                cmd.CommandText += "values(:1,:2,:3,'Y',:4,:5,:6,1)";
                cmd.Parameters.Add(null, attrKey);
                cmd.Parameters.Add(null, Convert.ToInt32(r["language_key"]));
                cmd.Parameters.Add(null, attrPkey);
                cmd.Parameters.Add(null, name);
                cmd.Parameters.Add(null, type);
                cmd.Parameters.Add(null, description);
                Util.ExecuteCommand(cmd);
                cmd.Parameters.Clear();
            }
        }
        return attrKey;
    }

    #endregion

    }
}

