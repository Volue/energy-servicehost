/**************************************************************************
*******	    Database Interface Derived Class Implementation File    *******
*******		     Copyright Scanvest Olivetti a.s		    *******
***************************************************************************
*
* File name....: TBExTseriVl.C
* Version......: %I%
* Last modified: 92/08/24 18:46
* Path name....: %P%
* Created by...: Jarand Roeynstrand
* Created date.: 92/08/24 18:46
*
* DESCRIPTION:
*	Class implementation file for derived class TBExTseriVl.
*
* MODIFICATION HISTORY:
*	Version	Date		Sign	Description
*	1.0	92/08/24 18:46	jr	New file.
*
***************************************************************************
*/

#include "TBExTseriVl.h"

N O T   T O    B E  U S E D   B Y   A N Y O N E  ! ! ! !   JR!

// C o n s t r u c t o r s

TBExTseriVl :: TBExTseriVl (pLong timsKey,
			pLong stabyte,
			pLong numbbyte,
			pString chval,
			pString timeserCode,
			pString timeserName,
			pString tsquaVlChval,
			OS_BOOL fetchTypeCode) :
TBTseriVl (	timsKey,
			stabyte,
			numbbyte,
			chval)
{
	SetTimeserCode( timeserCode );
	SetTimeserName( timeserName );
	SetTsquaVlChval( tsquaVlChval );
	SetFetchTypeCode( fetchTypeCode );
}

TBExTseriVl :: TBExTseriVl (const pTBExTseriVl TseriVl) : TBTseriVl((pTBTseriVl)TseriVl)
{
}

TBExTseriVl :: ~TBExTseriVl()
{
}

// S e t  f u n c t i o n s,   a l l   f i e l d s

void TBExTseriVl :: SetTimeserCode (const pString timeserCode)
{
	if (timeserCode)
		TBExTseriVl :: timeserCode = *timeserCode;
	else
		TBExTseriVl :: timeserCode.Nullify();
}

void TBExTseriVl :: SetTimeserName (const pString timeserName)
{
	if (timeserName)
		TBExTseriVl :: timeserName = *timeserName;
	else
		TBExTseriVl :: timeserName.Nullify();
}

void TBExTseriVl :: SetTsquaVlChval (const pString tsquaVlChval)
{
	if (tsquaVlChval)
		TBExTseriVl :: tsquaVlChval = *tsquaVlChval;
	else
		TBExTseriVl :: tsquaVlChval.Nullify();
}

void TBExTseriVl :: SetFetchTypeCode (const OS_BOOL fetchTypeCode)
{
	TBExTseriVl :: fetchTypeCode = fetchTypeCode;
}


// C l e a r   a l l   f i e l d s .

void TBExTseriVl :: Clear()
{
	TBTseriVl :: Clear();
	SetTimeserCode(NULL);
	SetTimeserName(NULL);
	SetTsquaVlChval(NULL);
	SetFetchTypeCode(STATE_FALSE);
}


//	D B S e l e c t   F u n c t i o n .

DB_RETURN TBExTseriVl :: DBSelect (DB_READ_MODE mode, int cursorId)
{
	DB_RETURN dberr;
	int doTheSelect;
	static int cursorIdOwner;


	// Temporaries for index fields:
	OSLong	tempTimsKey;
	OSLong	tempStabyte;


	dbi->ClearDBError();


	if (READ_ABORT == mode )
	{
		if (DBI_NO_CURSOR != cursorIdOwner)
		{
			dbi->FreeCursor( cursorIdOwner );
			cursorIdOwner = DBI_NO_CURSOR;
		}
		return REQUEST_OK;
	}

	if (READ_CURRENT == mode )
	{
		cursorIdOwner = DBI_NO_CURSOR;


		 // Select part for timeser.
		tempTimsKey	= GetTimsKey();

		if ( fetchTypeCode ||
			( ( ! timeserCode.IsNull() ) && ( tempTimsKey.IsNull() ) ) ||
			( ( ! timeserName.IsNull() ) && ( tempTimsKey.IsNull() ) )  )
		{


			// Get a new cursorid if necessary.
			if ( DBI_NO_CURSOR == cursorIdOwner )
			{
				cursorIdOwner = dbi->GetCursor();
				if ( DBI_NO_CURSOR == cursorIdOwner )
					return DB_ERROR;
			}


			if (   ( ( ! timeserCode.IsNull() ) && ( tempTimsKey.IsNull() ) ) || 
				( ( ! timeserName.IsNull() ) && ( tempTimsKey.IsNull() ) ) )
			{
				timeser.Clear();
				doTheSelect = 0;



				// Using type-code timeserCode:
				if ( ( ! doTheSelect ) && ( ! timeserCode.IsNull() )  &&
							( tempTimsKey.IsNull() ) )
				{
					timeser.SetCode( GetTimeserCode() );
					doTheSelect++;
				}



				// Using type-code timeserName:
				if ( ( ! doTheSelect ) && ( ! timeserName.IsNull() )  &&
							( tempTimsKey.IsNull() ) )
				{
					timeser.SetName( GetTimeserName() );
					doTheSelect++;
				}

				if ( doTheSelect )
				{
					dberr = timeser.DBSelect( mode, cursorIdOwner );
					if ( REQUEST_OK != dberr )
					{
						dbi->FreeCursor( cursorIdOwner );
						cursorIdOwner = DBI_NO_CURSOR;

						if ( DBI_NO_ROWS_FOUND == dbi->GetDBErrorNumber() )
						{
							dbi->ClearDBError();
							return dbi->SetDBError( DBI_REFERENCED_ROW_DOES_NOT_EXIST );
						}
						else
							return dberr;
					}
					SetTimsKey( timeser.GetTimsKey() );
				}

			}

		}



		 // Select part for tsquaVl.
		tempTimsKey	= GetTimsKey();
		tempStabyte	= GetStabyte();

		if ( fetchTypeCode ||
			( ( ! tsquaVlChval.IsNull() ) && ( tempTimsKey.IsNull() ) ) ||
			( ( ! tsquaVlChval.IsNull() ) && ( tempStabyte.IsNull() ) )  )
		{


			// Get a new cursorid if necessary.
			if ( DBI_NO_CURSOR == cursorIdOwner )
			{
				cursorIdOwner = dbi->GetCursor();
				if ( DBI_NO_CURSOR == cursorIdOwner )
					return DB_ERROR;
			}


			if (   ( ( ! tsquaVlChval.IsNull() ) && ( tempTimsKey.IsNull() ) ) || 
				( ( ! tsquaVlChval.IsNull() ) && ( tempStabyte.IsNull() ) ) )
			{
				tsquaVl.Clear();
				doTheSelect = 0;



				// Using type-code tsquaVlChval:
				if ( ( ! doTheSelect ) && ( ! tsquaVlChval.IsNull() )  &&
							( tempTimsKey.IsNull() ) &&
							( tempStabyte.IsNull() ) )
				{
					tsquaVl.SetChval( GetTsquaVlChval() );
					doTheSelect++;
				}

				if ( doTheSelect )
				{
					dberr = tsquaVl.DBSelect( mode, cursorIdOwner );
					if ( REQUEST_OK != dberr )
					{
						dbi->FreeCursor( cursorIdOwner );
						cursorIdOwner = DBI_NO_CURSOR;

						if ( DBI_NO_ROWS_FOUND == dbi->GetDBErrorNumber() )
						{
							dbi->ClearDBError();
							return dbi->SetDBError( DBI_REFERENCED_ROW_DOES_NOT_EXIST );
						}
						else
							return dberr;
					}
					SetTimsKey( tsquaVl.GetTimsKey() );
					SetStabyte( tsquaVl.GetStabyte() );
				}

			}

		}

	}

	// Common code for all modes.

	dberr = TBTseriVl :: DBSelect( mode, cursorId );
	if ( REQUEST_OK != dberr )
	{
		if ( DBI_NO_CURSOR != cursorIdOwner )
		{
			dbi->FreeCursor( cursorIdOwner );
			cursorIdOwner = DBI_NO_CURSOR;
		}
		return dberr;
	}

	if ( fetchTypeCode )
	{


		timeser.Clear();
		timeser.SetTimsKey( GetTimsKey() );
		dberr = timeser.DBSelect( READ_CURRENT, cursorIdOwner );

		// MUST-relation
		if ( REQUEST_OK != dberr )
		{
			dbi->FreeCursor ( cursorIdOwner );
			cursorIdOwner = DBI_NO_CURSOR;
			if ( DBI_NO_ROWS_FOUND == dbi->GetDBErrorNumber() )
			{
				dbi->ClearDBError();
				return dbi->SetDBError( DBI_REFERENCED_ROW_DOES_NOT_EXIST );
			}
			else
				return dberr;
		}
		SetTimeserCode( timeser.GetCode() );
		SetTimeserName( timeser.GetName() );


		tsquaVl.Clear();
		tsquaVl.SetTimsKey( GetTimsKey() );
		tsquaVl.SetStabyte( GetStabyte() );
		dberr = tsquaVl.DBSelect( READ_CURRENT, cursorIdOwner );

		// MUST-relation
		if ( REQUEST_OK != dberr )
		{
			dbi->FreeCursor ( cursorIdOwner );
			cursorIdOwner = DBI_NO_CURSOR;
			if ( DBI_NO_ROWS_FOUND == dbi->GetDBErrorNumber() )
			{
				dbi->ClearDBError();
				return dbi->SetDBError( DBI_REFERENCED_ROW_DOES_NOT_EXIST );
			}
			else
				return dberr;
		}
		SetTsquaVlChval( tsquaVl.GetChval() );
	}


	return REQUEST_OK;
}


//	D B U p d a t e   F u n c t i o n .

DB_RETURN TBExTseriVl :: DBStore ( int cursorId)
{
	DB_RETURN dberr;
	int doTheSelect;


	// Temporaries for index fields:
	OSLong	tempTimsKey;
	OSLong	tempStabyte;


	dbi->ClearDBError();



	 // Select part for timeser.	 // ( MUST-relation )
	tempTimsKey	= GetTimsKey();

	timeser.Clear();
	doTheSelect = 0;

	if ( ( ! tempTimsKey.IsNull() ) )
	{
		// Check if owner row exists using key
		timeser.SetTimsKey ( GetTimsKey() );
		doTheSelect++;
	}
	else
	{
		if ( ! timeserCode.IsNull() )	// Type-code timeserCode
		{
			timeser.SetCode( GetTimeserCode() );
			doTheSelect++;
		}

		if ( ! timeserName.IsNull() )	// Type-code timeserName
		{
			timeser.SetName( GetTimeserName() );
			doTheSelect++;
		}

	}

	if ( doTheSelect )
	{
		dberr = timeser.DBSelect( READ_CURRENT, cursorId );
		if ( REQUEST_OK != dberr )
		{
			if ( DBI_NO_ROWS_FOUND == dbi->GetDBErrorNumber() )
			{
				dbi->ClearDBError();
				return dbi->SetDBError( DBI_REFERENCED_ROW_DOES_NOT_EXIST );
			}
			else
				return dberr;
		}

		if ( tempTimsKey.IsNull() ) SetTimsKey( timeser.GetTimsKey() );

	}
	else
	{
		return dbi->SetDBError(DBI_DB_PARMS_NOT_INITIALIZED);
	}



	 // Select part for tsquaVl.	 // ( MUST-relation )
	tempTimsKey	= GetTimsKey();
	tempStabyte	= GetStabyte();

	tsquaVl.Clear();
	doTheSelect = 0;

	if ( ( ! tempTimsKey.IsNull() )	&&
	     ( ! tempStabyte.IsNull() ) )
	{
		// Check if owner row exists using key
		tsquaVl.SetTimsKey ( GetTimsKey() );
		tsquaVl.SetStabyte ( GetStabyte() );
		doTheSelect++;
	}
	else
	{
		if ( ! tsquaVlChval.IsNull() )	// Type-code tsquaVlChval
		{
			tsquaVl.SetChval( GetTsquaVlChval() );
			doTheSelect++;
		}

	}

	if ( doTheSelect )
	{
		dberr = tsquaVl.DBSelect( READ_CURRENT, cursorId );
		if ( REQUEST_OK != dberr )
		{
			if ( DBI_NO_ROWS_FOUND == dbi->GetDBErrorNumber() )
			{
				dbi->ClearDBError();
				return dbi->SetDBError( DBI_REFERENCED_ROW_DOES_NOT_EXIST );
			}
			else
				return dberr;
		}

		if ( tempTimsKey.IsNull() ) SetTimsKey( tsquaVl.GetTimsKey() );

		if ( tempStabyte.IsNull() ) SetStabyte( tsquaVl.GetStabyte() );

	}
	else
	{
		return dbi->SetDBError(DBI_DB_PARMS_NOT_INITIALIZED);
	}



	// Do the update.
	return TBTseriVl :: DBStore( cursorId );
}


//	D B D e l e t e   F u n c t i o n .

DB_RETURN TBExTseriVl :: DBDelete ( int cursorId )
{
	DB_RETURN dberr;
	int doTheSelect;


	// Temporaries for index fields:
	OSLong	tempTimsKey;
	OSLong	tempStabyte;




	dbi->ClearDBError();

	// Check if where-clause is in use:
	if ( GetWhere() )
	{
		SetWhere((char *)NULL);
		return dbi->SetDBError( DBI_DELETE_WITH_WHERE_CLAUSE_NOT_IMPLEMENTED );
	}


	if ( ( tempTimsKey.IsNull() ) || ( tempStabyte.IsNull() ) )
	{


		// Select part for timeser. ( MUST - relation)
		tempTimsKey	= GetTimsKey();

		doTheSelect = 0;

		if ( (  tempTimsKey.IsNull() ) )
		{
			// Check if type-code is set
			if ( ! ( doTheSelect || timeserCode.IsNull() ) ) // Type-code Code:
			{
				timeser.Clear();
				timeser.SetCode( GetTimeserCode() );
				doTheSelect++;
			}

			if ( ! ( doTheSelect || timeserName.IsNull() ) ) // Type-code Name:
			{
				timeser.Clear();
				timeser.SetName( GetTimeserName() );
				doTheSelect++;
			}

		}
		else
		{
			timeser.SetTimsKey( GetTimsKey() );
			doTheSelect++;
		}


		if ( doTheSelect )
		{
			dberr = timeser.DBSelect( READ_CURRENT, cursorId );
			if ( REQUEST_OK != dberr )
			{
				if ( DBI_NO_ROWS_FOUND == dbi->GetDBErrorNumber() )
				{
					dbi->ClearDBError();
					return dbi->SetDBError( DBI_REFERENCED_ROW_DOES_NOT_EXIST );
				}
				else
				{
					return dberr;
				}
			}
			else
			{
				if ( tempTimsKey.IsNull() ) SetTimsKey( timeser.GetTimsKey() );
			}
		}
	}



	if ( ( tempTimsKey.IsNull() ) || ( tempStabyte.IsNull() ) )
	{


		// Select part for tsquaVl. ( MUST - relation)
		tempTimsKey	= GetTimsKey();
		tempStabyte	= GetStabyte();

		doTheSelect = 0;

		if ( (  tempTimsKey.IsNull() ) || (  tempStabyte.IsNull() ) )
		{
			// Check if type-code is set
			if ( ! ( doTheSelect || tsquaVlChval.IsNull() ) ) // Type-code Chval:
			{
				tsquaVl.Clear();
				tsquaVl.SetChval( GetTsquaVlChval() );
				doTheSelect++;
			}

		}
		else
		{
			tsquaVl.SetTimsKey( GetTimsKey() );
			tsquaVl.SetStabyte( GetStabyte() );
			doTheSelect++;
		}


		if ( doTheSelect )
		{
			dberr = tsquaVl.DBSelect( READ_CURRENT, cursorId );
			if ( REQUEST_OK != dberr )
			{
				if ( DBI_NO_ROWS_FOUND == dbi->GetDBErrorNumber() )
				{
					dbi->ClearDBError();
					return dbi->SetDBError( DBI_REFERENCED_ROW_DOES_NOT_EXIST );
				}
				else
				{
					return dberr;
				}
			}
			else
			{
				if ( tempTimsKey.IsNull() ) SetTimsKey( tsquaVl.GetTimsKey() );
				if ( tempStabyte.IsNull() ) SetStabyte( tsquaVl.GetStabyte() );
			}
		}
	}



	// Check for existence of tsquaVl row.
	tsquaVl.Clear();
	tsquaVl.SetTimsKey( GetTimsKey() );
	tsquaVl.SetStabyte( GetStabyte() );

	dberr = tsquaVl.DBSelect( READ_CURRENT, cursorId );
	if ( REQUEST_OK == dberr )
	{
		dbi->ClearDBError();
		return dbi->SetDBError( DBI_REFERENCES_TO_ROW_EXIST );
	}



	// Do the delete.
	return TBTseriVl :: DBDelete( cursorId );
}
// end_of_file
