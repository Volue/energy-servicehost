using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;
using Powel.Icc.Common;
using Powel.Icc.Services.Time;
using Powel.Icc.Data.Entities.Metering;

namespace Powel.Icc.Data.Metering
{
	/// <summary>
	/// Class for accessing Instrument-related data tables
	/// </summary>
	public class ComponentData
	{
		public static Component GetLatestById(string componentId, ComponentType componentType, IDbConnection connection)
		{
			ArrayList alComponents = GetById(componentId, componentType, UtcTime.Null, connection);
			UtcTime latest = UtcTime.MinValue;
			Component latestComp = null;
			foreach (Component component in alComponents)
			{
				if (component.ValidFromDate > latest)
				{
					latest = component.ValidFromDate;
					latestComp = component;
				}
			}
			return latestComp;
		}

		public static ArrayList GetById(string componentId, ComponentType componentType, UtcTime validAtTime, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.ComponentGetByName");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("ICompID", OracleDbType.Varchar2, componentId, ParameterDirection.Input);
            cmd.Parameters.Add("ICompType", OracleDbType.Int32, GetInstrumentTypeFromComponentType(componentType), ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime), ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);

			return GetComponentsFromCursor((OracleRefCursor)cmd.Parameters[0].Value, validAtTime, connection);
		} //GetById

		public static ArrayList GetByKey(int comp_key, UtcTime validAtTime, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.ComponentGetByKey");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("ICompKey", OracleDbType.Int32, comp_key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime), ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);

			return GetComponentsFromCursor((OracleRefCursor)cmd.Parameters[0].Value, validAtTime, connection);
		} //GetByKey

		/// <summary>
		/// Stores a new or updated component to DB. Related measure point must be provided as well.
		/// </summary>
		/// <param name="component">Component</param>
		/// <param name="relatedMeasurePoint">Measure point, which component is located at</param>
		/// <param name="timeOfUpdate">Time from which changes shall be effective</param>
		/// <param name="connection">database connection, if part of a transaction</param>
		/// <returns></returns>
		public static int Update(Component component, MeasurePoint relatedMeasurePoint, UtcTime timeOfUpdate, IDbConnection connection)
		{
			if (timeOfUpdate == UtcTime.Null)
				throw new ArgumentException("Time of update of a component should not be UtcTime.Null");
			if (relatedMeasurePoint != null)
				component.MeasKey = relatedMeasurePoint.Key;
			if (component.MasterName == String.Empty)
				SetDefaultMaster(component);

		    string fabrNo = component.GetAttributeString("FabrNo");
            int lotKey = Convert.ToInt32(component.GetAttributeString("LotKey"));

            RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.ComponentUpdate");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("nError", OracleDbType.Int32, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("IinstKey", OracleDbType.Int32, component.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IinstrNo", OracleDbType.Varchar2, component.Id, ParameterDirection.Input);
			cmd.Parameters.Add("IlotKey", OracleDbType.Int32, lotKey, ParameterDirection.Input);
			cmd.Parameters.Add("Iname", OracleDbType.Varchar2, null, ParameterDirection.Input);
			cmd.Parameters.Add("Idescript", OracleDbType.Varchar2, null, ParameterDirection.Input);
			cmd.Parameters.Add("Istate", OracleDbType.Int32, component.State, ParameterDirection.Input);
			cmd.Parameters.Add("Imaster", OracleDbType.Varchar2, component.MasterName, ParameterDirection.Input);
			cmd.Parameters.Add("IinstTyp", OracleDbType.Int32, GetInstrumentTypeFromComponentType(component.ComponentType), ParameterDirection.Input);
            cmd.Parameters.Add("IfabrNo", OracleDbType.Varchar2, fabrNo, ParameterDirection.Input);
			cmd.Parameters.Add("Ilocation", OracleDbType.Varchar2, component.Location, ParameterDirection.Input);
			cmd.Parameters.Add("ImeasKey", OracleDbType.Int32, component.MeasKey, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(timeOfUpdate), ParameterDirection.Input);
			cmd.Parameters.Add("Isignature", OracleDbType.Varchar2, null, ParameterDirection.Input);

            if (component is Transformer)
            {
                Transformer t = component as Transformer;
                if (t.Phase != 0)
                {
                    cmd.Parameters.Add("Iphase", OracleDbType.Int32, t.Phase, ParameterDirection.Input);
                }
            }

			MeteringData.ExecuteMeteringDBCommand(cmd, connection);

			int n = Util.GetInt32(cmd.Parameters["nError"]);
			if (n > 0)
			{
				component.Key = n; //update key if new component
				SetAttributes(component, connection);
			}
			return n;
		} //Update

		/// <summary>
		/// Retrieves default master key from DB, and sets value on given component object.
		/// </summary>
		/// <param name="component"></param>
		public static void SetDefaultMaster(Component component)
		{
			if (component != null)
			{
				string masterName = "";

				if (component is Meter)
				{
					masterName = IccConfiguration.Component.MasterKeyMeter;
				}
				else if (component is Terminal)
				{
					masterName = IccConfiguration.Component.MasterKeyTerminal;
				}
				else if (component is Transformer)
				{
					if (((Transformer)component).TrafoType == TransformerType.CURRENT)
						masterName = IccConfiguration.Component.MasterKeyTransformerCurrent;
					else if (((Transformer)component).TrafoType == TransformerType.VOLTAGE)
						masterName = IccConfiguration.Component.MasterKeyTransformerVoltage;
				}

				component.MasterName = masterName;
			}
		}
				
		private static ArrayList GetComponentsFromCursor(OracleRefCursor refCursor, UtcTime validAtTime, IDbConnection connection)
		{
			ArrayList alComponents = new ArrayList();
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			Component component;
			int key, measKey,lotKey;
			string mstrName;
			UtcTime validFromTime, validToTime;
            string instrNo, location, fabrNo;
			ComponentState state;

			using (refCursor)
			{
				using (OracleDataReader reader = refCursor.GetDataReader())
				{
					while (reader.Read())
					{
						ComponentType type = GetComponentTypeFromInstrumentType(Util.GetInt32(reader, "inst_typ"));
						key = Util.GetInt32(reader, "inst_key");
						mstrName = Util.GetString(reader, "master_name");
						measKey = Util.GetInt32(reader, "meas_key");
						instrNo = Util.GetString(reader, "instr_no");
						validFromTime = cal.ToUtcTime(Util.GetString(reader, "valftime"));
						validToTime = cal.ToUtcTime(Util.GetString(reader, "valttime"));
						state = (ComponentState)Enum.ToObject(typeof(ComponentState), Util.GetInt32(reader, "stateSt"));
						location = Util.GetString(reader, "location");
                        fabrNo = Util.GetString(reader, "fabr_no");
                        lotKey = Util.GetInt32(reader, "lot_key");

						switch (type)
						{
							case ComponentType.METER:
								component = new Meter(key, mstrName, measKey, instrNo, validFromTime, validToTime, state, location, null, null); //temp comp
								ArrayList alRegisters = RegisterData.GetForComponent(component, validAtTime, connection);
								Terminal terminal = MeterData.GetTerminal(component, validAtTime, connection);
								component = new Meter(key, mstrName, measKey, instrNo, validFromTime, validToTime, state, location, terminal == null ? null : terminal.Id, (Register[])alRegisters.ToArray(typeof(Register)));
                                if (fabrNo != string.Empty)
                                { component.SetAttributeString("FabrNo", fabrNo); }
                                if (lotKey != 0)
                                { component.SetAttributeString("LotKey", lotKey.ToString()); }
								break;
							case ComponentType.TERMINAL:
								component = new Terminal(key, mstrName, measKey, instrNo, validFromTime, validToTime, state, location);
								break;
							case ComponentType.TRANSFORMER_CURRENT:
								component = new Transformer(key, mstrName, measKey, instrNo, validFromTime, validToTime, state, location, 0/*Set during GetAttributes*/, 0/*Set during GetAttributes*/, TransformerType.CURRENT);
								break;
							case ComponentType.TRANSFORMER_VOLTAGE:
								component = new Transformer(key, mstrName, measKey, instrNo, validFromTime, validToTime, state, location, 0/*Set during GetAttributes*/, 0/*Set during GetAttributes*/, TransformerType.VOLTAGE);
								break;
							default: //ignore other component types
								continue;
						}
						GetAttributes(component, connection);
						alComponents.Add(component);
					}
				}
			}
			return alComponents;
		}  // GetComponentsFromCursor

		public static ArrayList GetForMeasurePoint(MeasurePoint measurePoint, UtcTime validAtTime)
		{
			return GetForMeasurePoint(measurePoint, validAtTime, null);
		}

		public static ArrayList GetForMeasurePoint(MeasurePoint measurePoint, UtcTime validAtTime, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.ComponentGetForMeasurePoint");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("ImeasKey", OracleDbType.Int32, measurePoint.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime), ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);

			return GetComponentsFromCursor((OracleRefCursor)cmd.Parameters[0].Value, validAtTime, connection);
		}

		public static ArrayList GetForTerminal(Terminal terminal, UtcTime validAtTime, IDbConnection connection)
		{
			RegionalCalendar cal = IccConfiguration.Time.DatabaseCalendar;
			OracleCommand cmd = new OracleCommand("ICC_METERING.ComponentGetForTerminal");
			cmd.CommandType = CommandType.StoredProcedure;
			cmd.Parameters.Add("retval", OracleDbType.RefCursor, ParameterDirection.ReturnValue);
			cmd.Parameters.Add("IinstKey", OracleDbType.Int32, terminal.Key, ParameterDirection.Input);
			cmd.Parameters.Add("IvalTime", OracleDbType.Varchar2, cal.ToDatabaseString(validAtTime), ParameterDirection.Input);

			Util.ExecuteCommand(cmd, connection);

			return GetComponentsFromCursor((OracleRefCursor)cmd.Parameters[0].Value, validAtTime, connection);
		}

		public static MeasurePoint GetMeasurePoint(Component component, UtcTime validAtTime, IDbConnection connection)
		{
			if (validAtTime != UtcTime.Null)
				throw new ArgumentException("Time must be specified in call to ComponentData.GetMeasurePoint");
			List<MeasurePoint> measurePoints = MeasurePointData.GetByKey(component.MeasKey, validAtTime, connection);
			if (measurePoints.Count > 0) //shall never be more than one when time is specified
				return measurePoints[0];
			return null;
		}

		private static void SetAttributes(Component component, IDbConnection connection)
		{
			int nAttributesInt = component.AttributesInt.Count;
			if (nAttributesInt > 0)
			{
				int[] instrKeys = new int[nAttributesInt];
				for (int i = 0; i < nAttributesInt; i++)
					instrKeys[i] = component.Key;
				string[] sAttributes = new string[nAttributesInt];
				component.AttributesInt.Keys.CopyTo(sAttributes, 0);
				int[] values = new int[nAttributesInt];
				component.AttributesInt.Values.CopyTo(values, 0);
				OracleCommand cmd = new OracleCommand("ICC_METERING.SetInstrAttributeInt");
				cmd.CommandType = CommandType.StoredProcedure;
				cmd.ArrayBindCount = nAttributesInt;
				cmd.Parameters.Add("iInstKey", OracleDbType.Int32, instrKeys, ParameterDirection.Input);
				cmd.Parameters.Add("iAttribute", OracleDbType.Varchar2, sAttributes, ParameterDirection.Input);
				cmd.Parameters.Add("iValue", OracleDbType.Int32, values, ParameterDirection.Input);

				MeteringData.ExecuteMeteringDBCommand(cmd, connection);

				//TODO Set other attributes
			}
		}

		private static void GetAttributes(Component component, IDbConnection connection)
		{
			Powel.Icc.Data.ComponentData compData = new Data.ComponentData();
			int masterKey = compData.GetMasterKey(component.MasterName, connection);
			DataTable dtAttrs = compData.GetComponentsAttributes(component.Key.ToString(), masterKey.ToString());
			DataRow[] intRows = dtAttrs.Select("ivalue is not null");
			foreach (DataRow dr in intRows)
				component.SetAttributeInt((string)dr["name"], Convert.ToInt32(dr["ivalue"]));

			DataRow[] floatRows = dtAttrs.Select("rvalue is not null");
			foreach (DataRow dr in floatRows)
				component.SetAttributeFloat((string)dr["name"], float.Parse(dr["rvalue"].ToString()));

			DataRow[] stringRows = dtAttrs.Select("cvalue is not null");
			foreach (DataRow dr in stringRows)
				component.SetAttributeString((string)dr["name"], dr["cvalue"].ToString());
		}

		private static int GetInstrumentTypeFromMaster(string masterName)
		{
			OracleCommand cmd = new OracleCommand("SELECT INST_TYP FROM MASTER WHERE NAME = '" + masterName + "'");
			object o = Util.CommandToScalar(cmd);
			if (o == null)
				throw new IccException(10010, Convert.ToString(masterName));
			
			return (int)o;
		}

		private static ComponentType GetComponentTypeFromInstrumentType(int inst_typ)
		{
			if (inst_typ == ComponentTypeMeter)
				return ComponentType.METER;

			if (inst_typ == ComponentTypeTerminal)
				return ComponentType.TERMINAL;

			if (inst_typ == ComponentTypeTransformerCurrent)
				return ComponentType.TRANSFORMER_CURRENT;

			if (inst_typ == ComponentTypeTransformerVoltage)
				return ComponentType.TRANSFORMER_VOLTAGE;

			return ComponentType.UNKNOWN;
		}

		/// <summary>
		/// Method to retrive database value om component type. Shall throw exception when unknown type, as we don't want inst_typ 0 to be stored into DB.
		/// </summary>
		/// <param name="componentType"></param>
		/// <returns></returns>
		/// <throws>ArgumentEzxeption if invalid paramter is input</throws>
		static int GetInstrumentTypeFromComponentType(ComponentType componentType)
		{
			switch (componentType)
			{
				case ComponentType.METER:
					return ComponentTypeMeter;
				case ComponentType.TERMINAL:
					return ComponentTypeTerminal;
				case ComponentType.TRANSFORMER_CURRENT:
					return ComponentTypeTransformerCurrent;
				case ComponentType.TRANSFORMER_VOLTAGE:
					return ComponentTypeTransformerVoltage;
				default:
					throw new ArgumentException("ComponentData.GetComponentType(): Unknown component type");
			}
		}

		private static int componentTypeMeter;
		private static int ComponentTypeMeter
		{
			get
			{
				if (componentTypeMeter == 0)
					componentTypeMeter = GetInstrumentTypeFromMaster(
						IccConfiguration.Component.MasterKeyMeter);

				return componentTypeMeter;
			}
		}
		private static int componentTypeTerminal;
		private static int ComponentTypeTerminal
		{
			get
			{
				if (componentTypeTerminal == 0)
					componentTypeTerminal =
						GetInstrumentTypeFromMaster(
						IccConfiguration.Component.MasterKeyTerminal);

				return componentTypeTerminal;
			}
		}
		private static int componentTypeTransformerCurrent;
		private static int ComponentTypeTransformerCurrent
		{
			get
			{
				if (componentTypeTransformerCurrent == 0)
					componentTypeTransformerCurrent =
						GetInstrumentTypeFromMaster(
						IccConfiguration.Component.MasterKeyTransformerCurrent);

				return componentTypeTransformerCurrent;
			}
		}
		private static int componentTypeTransformerVoltage;
		private static int ComponentTypeTransformerVoltage
		{
			get
			{
				if (componentTypeTransformerVoltage == 0)
					componentTypeTransformerVoltage =
						GetInstrumentTypeFromMaster(
						IccConfiguration.Component.MasterKeyTransformerVoltage);

				return componentTypeTransformerVoltage;
			}
		}
	}
}
