/*** File_Header *** -*- C++ -*- *********************************************************
 *
 *	File name : DBInterf.h
 *	Author    : Jarand Roeynstrand    EFI           DATE: 920710
 *	Copyright : Norwegian Electric Power Research Institute
 *	Compiler  : AT&T C++ 2.1
 *
 *	Path         : %P%
 *	$Revision: 37171 $
 *	Last changed : $Date: 2004-09-29 17:43:15 +0000 (Wed, 29 Sep 2004) $
 *
 *	Contents : An headefile for an extension to the SO DBInterface class
 *	that supplies cursor Get and cursor Free functions. The original
 *	DBInterface class is renamed to DBInterfaceBasic.
 *	
 *	Includes and defines:
 */
#ifndef DBINTERF_H_EFI
#define DBINTERF_H_EFI

#include "DBInterfaceBasic.h"

#include "DBILineFile.h"

#define MAX_UNUSED_CURSORS 4
/*	Externals and statics:
 */

/*****************************************************************************/

class DBMS_API DBInterface : public DBInterfaceBasic
{
    public:

	// Constructor/Destructor.
	DBInterface();
	~DBInterface();
    
	// Functions to get free cursors, and dispose of cursors no longer needed.
	int 	GetCursor();
	void 	FreeCursor( int &cursorId );

	// Cleanup and init functions.
	void	InitCursors();
	void	ResetCursors();
	
	// Functions for DBSelect counter
	void 	AddNumberOfRowsFound( const int cursorId );
	int 	NumberOfRowsFound( const int cursorId );

    private:

	// Stack of cursors that are open, but not in use.
	int freeStack[MAX_UNUSED_CURSORS];
	int stackPt;
};
extern DBMS_API DBInterface *dbi; // put it here to get it right in dll
#endif
    
