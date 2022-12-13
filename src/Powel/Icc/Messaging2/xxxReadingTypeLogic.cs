using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using Powel.Icc.Data;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Data.Metering;
using Powel.Icc.Interop;
using Powel.Icc.Messaging2.MeteringXML;

namespace Powel.Icc.Messaging2
{
    public class ReadingTypeLogic
    {
        public static ReadingType GetReadingType(IECReadingTypeType rt)
        {
            return ReadingTypeData.GetReadingType(rt.TimeAttribute.ToString(),
                                                  rt.DataQualifier.ToString(),
                                                  rt.AccumulationBehaviour.ToString(),
                                                  rt.FlowDirection.ToString(),
                                                  rt.UomCategory.ToString(), 
                                                  rt.MeasurementCategory.ToString(),
                                                  rt.Phase.ToString(), 
                                                  rt.Multiplier.ToString(),
                                                  rt.UnitOfMeasurement.ToString());
        }
    }
}
