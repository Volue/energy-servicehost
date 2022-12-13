using NUnit.Framework;
using System;

using TASE2.Library.Common;

namespace tests
{
	[TestFixture ()]
	public class Test
	{
		[Test ()]
		public void TestDataStateGetters ()
		{
			DataState state = 0;

			Assert.AreEqual (state.GetStateValue (), StateValue.BETWEEN);
			Assert.AreEqual (state.GetValidity (), Validity.VALID);
			Assert.AreEqual (state.GetCurrentSource (), Source.TELEMETERED);
			Assert.AreEqual (state.GetNormalValue (), NormalValue.NORMAL);

			state = DataState.CURRENT_SOURCE_HI | DataState.CURRENT_SOURCE_LO;

			Assert.AreEqual (state.GetStateValue (), StateValue.BETWEEN);
			Assert.AreEqual (state.GetValidity (), Validity.VALID);
			Assert.AreEqual (state.GetCurrentSource (), Source.ESTIMATED);
			Assert.AreEqual (state.GetNormalValue (), NormalValue.NORMAL);

			state = DataState.LO | DataState.CURRENT_SOURCE_LO | DataState.VALIDITY_HI;

			Assert.AreEqual (state.GetStateValue (), StateValue.OFF);
			Assert.AreEqual (state.GetValidity (), Validity.SUSPECT );
			Assert.AreEqual (state.GetCurrentSource (), Source.CALCULATED);
			Assert.AreEqual (state.GetNormalValue (), NormalValue.NORMAL);

			state = DataState.LO | DataState.HI | DataState.CURRENT_SOURCE_LO | DataState.VALIDITY_HI | DataState.NORMAL_VALUE;

			Assert.AreEqual (state.GetStateValue (), StateValue.INVALID);
			Assert.AreEqual (state.GetValidity (), Validity.SUSPECT );
			Assert.AreEqual (state.GetCurrentSource (), Source.CALCULATED);
			Assert.AreEqual (state.GetNormalValue (), NormalValue.ABNORMAL);
		}
	}
}

