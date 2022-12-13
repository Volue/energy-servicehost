using System;
using TASE2.Library.Common;
using TASE2.Library.Server;

namespace libtase2.NET.server
{
    public class SentPointValue
    {
        private DataPoint dataPoint = null;
        private PointValue pointValue = null;

        internal SentPointValue(DataPoint dataPoint, PointValue pointValue)
        {
            this.dataPoint = dataPoint;
            this.pointValue = pointValue;
        }

        public DataPoint DataPoint
        {
            get
            {
                return this.dataPoint;
            }
        }

        public PointValue PointValue
        {
            get
            {
                return this.pointValue;
            }
        }
    }
}
