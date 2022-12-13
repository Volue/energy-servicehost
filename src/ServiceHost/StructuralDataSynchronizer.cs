using System;
using System.Collections;
using System.Configuration;
using System.Data;
using System.Diagnostics;
using Powel.Icc.Common;
using Powel.Icc.Data;
using Powel.Icc.Data.Entities.Metering;
using Powel.Icc.Diagnostics;
using Powel.Icc.Interop;
using Powel.Icc.Metering;
using Powel.Icc.Process;
using Powel.Time;
using Powel.Icc.Metering.InstallationXML;

namespace Powel.Icc.Messaging
{
	class StructuralDataSynchronizer : ServiceIterationBase
	{
		public StructuralDataSynchronizer(
			EventLog eventLog, EventLogModuleItem iccLog) 
			: base(eventLog, iccLog)
		{
		}

		public override string ServiceIterationName 
		{ 
			get
			{
				return "StructuralDataSyncronizer";
			}
		}

		public override void Initialize()
		{}
		public override void RunIteration(out bool actualWorkDone)
		{
			actualWorkDone = false;
		}
	}
	
	
}
