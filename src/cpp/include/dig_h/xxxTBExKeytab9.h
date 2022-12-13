#ifndef TBExKeytab9_H
#define TBExKeytab9_H

#include "ExTable.h"
#include "TBKeytab9.h"

extern int dbiExCode;

// T y p e   D e f i n i t i o n s

class	TBExKeytab9;
typedef TBExKeytab9 * pTBExKeytab9;


// C l a s s   d e c l a r a t i o n

class TBExKeytab9 : public TBKeytab9,
public ExTable
{
public:
	TBExKeytab9	(	);
	TBExKeytab9	(	const pTBExKeytab9 exKeytab9 );
	~TBExKeytab9	();

	// Database interface functions.
	DB_RETURN DBSelect(DB_READ_MODE mode,int cursorId)
			{ return ExTable :: DBSelect(mode, cursorId); }
	DB_RETURN DBStore(int cursorId, DB_BATCH_MODE dbBatchMode = DB_NO_BATCH )
			{ return ExTable :: DBStore( cursorId, dbBatchMode); }
	DB_RETURN DBUpdate(int cursorId, DB_BATCH_MODE dbBatchMode = DB_NO_BATCH )
			{ return ExTable :: DBUpdate( cursorId, dbBatchMode); }
	DB_RETURN  DBInsert(int cursorId, DB_BATCH_MODE dbBatchMode = DB_NO_BATCH )
			{ return ExTable :: DBInsert( cursorId, dbBatchMode ); }
	DB_RETURN  DBDelete(int cursorId, DB_BATCH_MODE dbBatchMode = DB_NO_BATCH )
			{ return ExTable :: DBDelete( cursorId, dbBatchMode ); }

	// Clear function (to clear all fields).
	void Clear	(void) { ExTable :: Clear(); }
};
#endif	// TBExKeytab9_H
