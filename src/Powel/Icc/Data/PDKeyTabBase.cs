using System;
using Oracle.ManagedDataAccess.Client;
using Oracle.ManagedDataAccess.Types;

namespace Powel.Icc.Data
{

	/// <summary>
	/// Base class for both PDKeyTab9 and PDKeyTab so we don't have to update both. It
	/// requires that it exists a table and a sequence in the database that matches the
	/// table argument to the constructor.
	///
	/// It also have (more) correct interface that can't be used incorrectly. The old
	/// interface allowed to set noEntries but did not resize arrays, leading to out of
	/// bounds errors.
	///
	/// Once the execute method has been called the instance should not be reused, unsure
	/// what the effects of that would be.
	/// </summary>
	abstract public class PDKeyTabBase
	{
		readonly int noEntries;
		int sequenceNumber;
		readonly string table;
		int[] rcount;
		int[] seqno;
		OracleDecimal[] lValue1;
		OracleDecimal[] lValue2;
		string[] cValue;
		readonly string select_nextvalue;
		readonly string execute_insert;
		System.Data.IDbConnection connection;
		
		/// <summary>
		/// 
		/// </summary>
		/// <param name="table">the name of the table and sequence to use</param>
		/// <param name="noOfEntries">At this time we do preallocation of arrays, that means
		/// you need to tell us how many entries you are going to use</param> 
		/// <param name="con">We need this because else we would not be sure to get the same
		/// session that contains the currval for the sequence that is used in the trigger
		/// that is fired by inserting into keytab</param>
		protected PDKeyTabBase(string table, int noOfEntries, System.Data.IDbConnection con)
		{
			this.table = table;
			noEntries = noOfEntries;
			sequenceNumber = -1;
			rcount = new int[noEntries];
			seqno = new int[noEntries];
			lValue1 = new OracleDecimal[noEntries];
			lValue2 = new OracleDecimal[noEntries];
			cValue = new string[noEntries];
			connection = con;
			select_nextvalue = String.Format("select pdmisc.sequence_nextval('{0}_seq') as value from dual", table);
			execute_insert = String.Format("insert into {0} (RCOUNT,SEQNO,IVALUE1,IVALUE2,CVALUE) values(:rcount,:seqno,:lValue1,:lValue2,:cValue)", table);
		}
		
		/// <summary>
		///  there is no set method because that would mean we would need to reinit some other
		///  fields and copy data so we do not lose it.
		/// </summary>
		public int NoEntries
		{
			get { return noEntries; }
		}

		/// <summary>
		///   
		/// </summary>
		public string Table
		{
			get { return table; }
		}
		/// <summary>
		///  Setups and returns a new identifier that will be used by the trigger when
		///  inserting into keytab, repeated calls to this method is allowed, it will return
		///  the same number everytime.
		/// </summary>
		/// <returns></returns>
		public int fetchSequenceNumber()
		{
			if (sequenceNumber < 0) {
				OracleCommand cmd = new OracleCommand(select_nextvalue);
				sequenceNumber = Convert.ToInt32(Util.CommandToScalar(cmd, connection));
			}
			
			return sequenceNumber;
		}
		
		public void AddLongValue1( int pos, long value )
		{
			lValue1[pos] = (OracleDecimal)value;
		}
		
		public void AddLongValue2( int pos, long value )
		{
			lValue2[pos] = (OracleDecimal)value;
		}
		
		public void AddCharValue( int pos, string value )
		{
			cValue[pos] = value;
		}
	  
		public void Execute()
		{
			// fill the key arrays
			for (int row = 0; row < noEntries; row++) {
				rcount[row] = sequenceNumber;
				seqno[row] = row;
			}
			OracleCommand cmd = new OracleCommand(execute_insert);
			cmd.ArrayBindCount = noEntries;
			
			OracleParameter prmRcount = new OracleParameter("rcount", OracleDbType.Int32);
			prmRcount.Value = rcount;
			cmd.Parameters.Add(prmRcount);
			
			OracleParameter prmSeqno = new OracleParameter("seqno", OracleDbType.Int32);
			prmSeqno.Value = seqno;
			cmd.Parameters.Add(prmSeqno);
			
			OracleParameter prmLValue1 = new OracleParameter("lValue1", OracleDbType.Decimal);
			prmLValue1.Value = lValue1;
			cmd.Parameters.Add(prmLValue1);
			
			OracleParameter prmLValue2 = new OracleParameter("lValue2", OracleDbType.Decimal);
			prmLValue2.Value = lValue2;
			cmd.Parameters.Add(prmLValue2);
			
			OracleParameter prmCValue = new OracleParameter("cValue", OracleDbType.Char);
			prmCValue.Value = cValue;
			cmd.Parameters.Add(prmCValue);
			
			Util.ExecuteCommand(cmd, connection);
		}

	}
}
