using System;
using System.Collections;
using Powel.Icc.Data;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Data.Metering;
using Powel.Icc.Services.Time;
using System.Data;
using ComponentData = Powel.Icc.Data.Metering.ComponentData;

namespace Powel.Icc.Metering
{
	/// <summary>
	/// Summary description for TransformerLogic.
	/// </summary>
	public class TransformerLogic
	{
		public static Transformer[] GetTransformers(MeasurePoint measurePoint, UtcTime validAtTime, IDbConnection connection)
		{
			int nTransformerVoltage = 0;
			int nTransformerCurrent = 0;
			ArrayList alTransformers = new ArrayList();
			ArrayList alComponents = ComponentData.GetForMeasurePoint(measurePoint, validAtTime, connection);
			foreach( Component comp in alComponents)
			{
				if(comp is Transformer)
				{
					Transformer trans = comp as Transformer;
          // Due to QC3437: Allows more than one current or voltage transformer,
          // but returns just the first of each (instead of throwing an exception).
          if (trans.TrafoType == TransformerType.CURRENT)
          {
            nTransformerCurrent++;
            if (nTransformerCurrent == 1)
							alTransformers.Add(trans);
          }
          else if (trans.TrafoType == TransformerType.VOLTAGE)
          {
						nTransformerVoltage++;
						if (nTransformerVoltage == 1)
							alTransformers.Add(trans);
          }
          else
						throw new DataException("Erraneous transformer type found; id = " + comp.Id);
				}
			}
			return (Transformer[]) alTransformers.ToArray(typeof(Transformer));
		}
	}
}
