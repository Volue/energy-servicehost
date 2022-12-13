using System;
using NUnit.Framework;

namespace Powel.Icc.Interop.UnitTests
{
	/// <summary>
	/// Summary description for TestLogging.
	/// </summary>
	[TestFixture]
	public class TestLogging
	{
		[Test]
		public void Test()
		{
			Assert.AreEqual("{0}", 
				IccApplication.PrepareFormatString("%s", LogFormat.Printf));
			
			Assert.AreEqual("RCM, {0} syntaks feil funnet i {1}. Henting av rapport avbrutt.", 
				IccApplication.PrepareFormatString("RCM, %d syntaks feil funnet i %s. Henting av rapport avbrutt.", LogFormat.Printf));
			
			Assert.AreEqual("Periode er fra {0} til {1}", 
				IccApplication.PrepareFormatString("Periode er fra %s til %s", LogFormat.Printf));
			
			Assert.AreEqual("Tidsserie '{0}' importert fra '{1}' fra filen {2} : starttid: {3}, antall verdier: {4}", 
				IccApplication.PrepareFormatString("Tidsserie '%s' importert fra '%s' fra filen %s : starttid: %10.10s, antall verdier: %d", LogFormat.Printf));
			
			Assert.AreEqual("2% av 100 er {0}", 
				IccApplication.PrepareFormatString("2%% av 100 er %d", LogFormat.Printf));
			
			Assert.AreEqual("2% av 100 er {0}", 
				IccApplication.PrepareFormatString("2%% av 100 er %1", LogFormat.Positional));
			
			Assert.AreEqual("Tidspunkt: {0} {1} {2} kl. {3}00. Avbryter simulering.", 
				IccApplication.PrepareFormatString("Tidspunkt: %d %s %d kl. %.2d00. Avbryter simulering.", LogFormat.Printf));
			
			Assert.AreEqual("Låser verdier i perioden {0} - {1}, {2} time(r)", 
				IccApplication.PrepareFormatString("Låser verdier i perioden %01 - %02, %03 time(r)", LogFormat.Positional));
			
			Assert.AreEqual("Arbeidsordre '{0}-{1}' er ikke signert.", 
				IccApplication.PrepareFormatString("Arbeidsordre '%01-%02' er ikke signert.", LogFormat.Positional));

			Assert.AreEqual("Feil verdi({1}) for argumentet '{0}'.", 
				IccApplication.PrepareFormatString("Feil verdi(%02) for argumentet '%01'.", LogFormat.Positional));

			Assert.AreEqual("Feil verdi({34555}) for argumentet '{88}'.", 
				IccApplication.PrepareFormatString("Feil verdi(%34556) for argumentet '%000000089'.", LogFormat.Positional));
		}
	}
}
