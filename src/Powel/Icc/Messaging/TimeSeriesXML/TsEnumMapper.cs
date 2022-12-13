using System;
using Powel.Icc.Services.DataContracts;

namespace Powel.Icc.TimeSeries.TimeSeriesXML
{
	/// <summary>
	/// Summary description for TsEnumMapper.
	/// </summary>
	public abstract class TsXMLEnumMapper
	{

		/*public enum ValMethod
		{
			ABS_LIMIT = 1,
			DELTA_LIMIT = 2,
			DELTA_CHANGE = 3,
			REP_VALUE = 4,
			CTRL_METER = 5,
			VAL_METER_SUM = 6,
			CTRL_HT = 28,
			CTRL_T = 29,
			CTRL_U = 30,
			CTRL_IMPORT = 31
		}*/



		public static ValidationMethods GetValMethod(TsValMethodType valMethod)
		{
			switch(valMethod)
			{
				case TsValMethodType.ABS_LIMIT:
					return ValidationMethods.AbsoluteLimit; //1;
				case TsValMethodType.CTRL_HT:
					return ValidationMethods.ControlHighTariffPeriod;// 28;
				case TsValMethodType.CTRL_IMPORT:
					return ValidationMethods.ControlImport; //31;
				case TsValMethodType.CTRL_METER:
					return ValidationMethods.ControlMeter; //5;
				case TsValMethodType.CTRL_T:
					return ValidationMethods.ControlTariff; //29;
				case TsValMethodType.CTRL_U:
					return ValidationMethods.ControlAccessibleEnergy; //30;
				case TsValMethodType.DELTA_CHANGE:
					return ValidationMethods.DeltaEpsilon; //3;
				case TsValMethodType.DELTA_LIMIT:
					return ValidationMethods.DeltaLimit; //2;
				case TsValMethodType.REP_VALUE:
					return ValidationMethods.RepeatValue; //4;
				case TsValMethodType.VAL_METER_SUM:
					return ValidationMethods.MeterSum; //6;
				default:
					throw new ArgumentException("Invalid validation method.");
			}
		}

		/*public static TsValMethodType GetValMethodXML(Quality.ValidationMethods valMethod)
		{
			switch(valMethod)
			{
				case 1:
					return TsValMethodType.ABS_LIMIT;
				case 28:
					return TsValMethodType.CTRL_HT;
				case 31:
					return TsValMethodType.CTRL_IMPORT;
				case 5:
					return TsValMethodType.CTRL_METER;
				case 29:
					return TsValMethodType.CTRL_T;
				case 30:
					return TsValMethodType.CTRL_U;
				case 3:
					return TsValMethodType.DELTA_CHANGE;
				case 2:
					return TsValMethodType.DELTA_LIMIT;
				case 4:
					return TsValMethodType.REP_VALUE;
				case 6:
					return TsValMethodType.VAL_METER_SUM;
				default:
					throw new Exception("Invalid validation method: " + valMethod);
			}
		}*/

		public static CorrectionMethods GetCorMethod(TsCorMethodType corMethod)
		{
			switch(corMethod)
			{
				case TsCorMethodType.CONSTANT:
					return CorrectionMethods.Constant; //1;
				case TsCorMethodType.COPY:
					return CorrectionMethods.Copy; //2;
				case TsCorMethodType.METER_SUM:
					return CorrectionMethods.MeterSum; //10;
				case TsCorMethodType.ENERGY:
					return CorrectionMethods.Energy; //8;
				case TsCorMethodType.EXTRAPOLATE:
					return CorrectionMethods.Extrapolate; //4;
				case TsCorMethodType.INTERPOLATE:
					return CorrectionMethods.Interpolate; //3;
				case TsCorMethodType.MEAN_DELTA_TIME:
					return CorrectionMethods.MeanDeltaT; //9;
				case TsCorMethodType.MEAN:
					return CorrectionMethods.Mean; //5;
				default:
					throw new ArgumentException("Invalid correction method.");
			}
		}

		/*public static TsCorMethodType GetCorMethodXML(Quality.CorrectionMethods corMethod)
		{
			switch(corMethod)
			{
				case 1:
					return TsCorMethodType.CONSTANT;
				case 2:
					return TsCorMethodType.COPYVALUE;
				case 3:
					return TsCorMethodType.INTERPOLATE;
				case 4:
					return TsCorMethodType.EXTRAPOLATE;
				case 5:
					return TsCorMethodType.MEANSIMULTS;
				case 8:
					return TsCorMethodType.ENERGYCONTROL;
				case 9:
					return TsCorMethodType.MEANSIMDELTAT;
				case 10:
					return TsCorMethodType.CORR_METER_SUM;
				default:
					throw new Exception("Invalid correction method: " + corMethod);
			}
		}*/
		
		/*public static TsUnit GetUnit( string unit)
		{
			switch( unit)
			{
				case TsUnit.None:
					return -32767;
				case TsUnit.Undefined:
					return 0;
				case TsUnit.M3:
					return 100;
				case TsUnit.Mm3:
					return 101;
				case TsUnit.Item:
					return 103;
				case TsUnit.MW:
					return 104;
				case TsUnit.GWh:
					return 105;
				case TsUnit.MWh:
					return 106;
				case TsUnit.Wh:
					return 107;
				case TsUnit.M3s:
					return 108;
				case TsUnit.Ls:
					return 109;
				case TsUnit.Lskm2:
					return 110;
				case TsUnit.M2:
					return 111;
				case TsUnit.Km2:
					return 112;
				case TsUnit.Degrees:
					return 113;
				case TsUnit.DegreesC:
					return 114;
				case TsUnit.Kg:
					return 115;
				case TsUnit.Pa:
					return 116;
				case TsUnit.V:
					return 117;
				case TsUnit.A:
					return 118;
				case TsUnit.Wm2:
					return 119;
				case TsUnit.Ms:
					return 120;
				case TsUnit.Sm:
					return 121;
				case TsUnit.Mgl:
					return 122;
				case TsUnit.Kgs:
					return 123;
				case TsUnit.Mm:
					return 124;
				case TsUnit.KWm:
					return 125;
				case TsUnit.MNOK:
					return 127;
				case TsUnit.NOKMWh:
					return 128;
				case TsUnit.MWhh:
					return 129;
				case TsUnit.Mg:
					return 130;
				case TsUnit.Cm:
					return 131;
				case TsUnit.Ofmax:
					return 132;
				case TsUnit.Ofnorm:
					return 133;
				case TsUnit.KWhh:
					return 140;
				case TsUnit.MWMJs:
					return 200;
				case TsUnit.T:
					return 201;
				case TsUnit.MJs:
					return 202;
				case TsUnit.Th:
					return 203;
				case TsUnit.NM3:
					return 204;
				case TsUnit.GJ:
					return 205;
				case TsUnit.PU:
					return 206;
				case TsUnit.KWh:
					return 800;
				case TsUnit.C100m:
					return 810;
				case TsUnit.Hours:
					return 811;
				case TsUnit.Hz:
					return 820;
				case TsUnit.MVA:
					return 821;
				case TsUnit.MVAr:
					return 822;
				case TsUnit.MVArh:
					return 823;
				case TsUnit.MVArhh:
					return 824;
				case TsUnit.MWHz:
					return 825;
				case TsUnit.KVAr:
					return 826;
				case TsUnit.KVArh:
					return 827;
				case TsUnit.KVArhh:
					return 828;
				case TsUnit.KPa:
					return 829;
				case TsUnit.M3h:
					return 830;
				case TsUnit.SEKMWh:
					return 831;
				case TsUnit.MWhC:
					return 832;
				case TsUnit.DegreesF:
					return 833;
				case TsUnit.KV:
					return 834;
				case TsUnit.KW:
					return 835;
				case TsUnit.W:
					return 836;
				case TsUnit.CZKMWh:
					return 837;
				case TsUnit.PLZMWh:
					return 838;
				case TsUnit.EURMWh:
					return 839;
				case TsUnit.DKKMWh:
					return 840;
				case TsUnit.RUBMWh:
					return 841;
				case TsUnit.DKKGJ:
					return 842;
				case TsUnit.NOK:
					return 1000;
				case TsUnit.SEK:
					return 1001;
				case TsUnit.DKK:
					return 1002;
				case TsUnit.FIM:
					return 1003;
				case TsUnit.DEM:
					return 1004;
				case TsUnit.RUB:
					return 1005;
				case TsUnit.NLG:
					return 1006;
				case TsUnit.CZK:
					return 1007;
				case TsUnit.PLZ:
					return 1008;
				case TsUnit.EUR:
					return 1999;
				default:
					return -32767;
			}
		}

		public static TsUnit GetUnitXML( int unit)
		{
			switch( unit)
			{
				case -32767:
					return TsUnit.None;
				case 0:
					return TsUnit.Undefined;
				case 100:
					return TsUnit.M3;
				case 101:
					return TsUnit.Mm3;
				case 103:
					return TsUnit.Item;
				case 104:
					return TsUnit.MW;
				case 105:
					return TsUnit.GWh;
				case 106:
					return TsUnit.MWh;
				case 107:
					return TsUnit.Wh;
				case 108:
					return TsUnit.M3s;
				case 109:
					return TsUnit.Ls;
				case 110:
					return TsUnit.Lskm2;
				case 111:
					return TsUnit.M2;
				case 112:
					return TsUnit.Km2;
				case 113:
					return TsUnit.Degrees;
				case 114:
					return TsUnit.DegreesC;
				case 115:
					return TsUnit.Kg;
				case 116:
					return TsUnit.Pa;
				case 117:
					return TsUnit.V;
				case 118:
					return TsUnit.A;
				case 119:
					return TsUnit.Wm2;
				case 120:
					return TsUnit.Ms;
				case 121:
					return TsUnit.Sm;
				case 122:
					return TsUnit.Mgl;
				case 123:
					return TsUnit.Kgs;
				case 124:
					return TsUnit.Mm;
				case 125:
					return TsUnit.KWm;
				case 126:
					return TsUnit.NOK; //Originally Norwegian crowns
				case 127:
					return TsUnit.MNOK;
				case 128:
					return TsUnit.NOKMWh;
				case 129:
					return TsUnit.MWhh;
				case 130:
					return TsUnit.Mg;
				case 131:
					return TsUnit.Cm;
				case 132:
					return TsUnit.Ofmax;
				case 133:
					return TsUnit.Ofnorm;
				case 140:
					return TsUnit.KWhh;
				case 200:
					return TsUnit.MWMJs;
				case 201:
					return TsUnit.T;
				case 202:
					return TsUnit.MJs;
				case 203:
					return TsUnit.Th;
				case 204:
					return TsUnit.NM3;
				case 205:
					return TsUnit.GJ;
				case 206:
					return TsUnit.PU;
				case 800:
					return TsUnit.KWh;
				case 810:
					return TsUnit.C100m;
				case 811:
					return TsUnit.Hours;
				case 820:
					return TsUnit.Hz;
				case 821:
					return TsUnit.MVA;
				case 822:
					return TsUnit.MVAr;
				case 823:
					return TsUnit.MVArh;
				case 824:
					return TsUnit.MVArhh;
				case 825:
					return TsUnit.MWHz;
				case 826:
					return TsUnit.KVAr;
				case 827:
					return TsUnit.KVArh;
				case 828:
					return TsUnit.KVArhh;
				case 829:
					return TsUnit.KPa;
				case 830:
					return TsUnit.M3h;
				case 831:
					return TsUnit.SEKMWh;
				case 832:
					return TsUnit.MWhC;
				case 833:
					return TsUnit.DegreesF;
				case 834:
					return TsUnit.KV;
				case 835:
					return TsUnit.KW;
				case 836:
					return TsUnit.W;
				case 837:
					return TsUnit.CZKMWh;
				case 838:
					return TsUnit.PLZMWh;
				case 839:
					return TsUnit.EURMWh;
				case 840:
					return TsUnit.DKKMWh;
				case 841:
					return TsUnit.RUBMWh;
				case 842:
					return TsUnit.DKKGJ;
				case 1000:
					return TsUnit.NOK;
				case 1001:
					return TsUnit.SEK;
				case 1002:
					return TsUnit.DKK;
				case 1003:
					return TsUnit.FIM;
				case 1004:
					return TsUnit.DEM;
				case 1005:
					return TsUnit.RUB;
				case 1006:
					return TsUnit.NLG;
				case 1007:
					return TsUnit.CZK;
				case 1008:
					return TsUnit.PLZ;
				case 1999:
					return TsUnit.EUR;
				default:
					throw new Exception("Invalid unit: " + unit);
			}
		}*/


	}
}
