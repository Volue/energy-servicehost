/**** File_Header *************************************************************
 *
 * %Z% Version   : %I%  Date:%E%  Time:%U%
 *
 *	File name : %M%
 *	Author    : Odd Broenner    EFI-20     DATE: 960216
 *	Copyright : Norwegian Electric Power Research Institute
 *	Compiler  : gcc
 *
 *      Keywords  : 
 *	Contents  : 
 *
 *	Includes and defines:
 */
#ifndef _DBHOSTVARIABLE_H_
#define _DBHOSTVARIABLE_H_

//
//	X11/Xlib.h defines Status as int. Status is a member function
//	of class DBHostVariable.
//	
#ifdef Status
#define _pd123_ Status
#undef Status
#endif
#include "dbmsapi.h"
/*	Externals and statics:
 */

/*	Function prototypes
 */

/*****************************************************************************/

class DBMS_API DBHostVariable {
// NEVER to be called !
    DBHostVariable( const DBHostVariable &hv );
    DBHostVariable &operator=( const DBHostVariable &hv );
  public:
    
    virtual ~DBHostVariable();
    DBHostVariable (const int _datatype, 
		    const char * const _name, 
		    int &count, 
		    const int _isNull );

    void     Value (const void * const _value, const int _valueLen);




    void   SetScale( const int val)     { scale = val;}
    void   SetPrecision( const int val) { precision = val;}

    char  *Name( void ) { return name;}
    int    Datatype( void ) { return datatype; }
    int    Format ( void ) { return format; }
    int    Maxlength ( void ) { return maxlength; }
    int    Scale ( void ) { return scale; }
    int    Precision ( void ) { return precision; }
    int    Status ( void ) { return status; }
    void   *Value( void )   { return (void*)value; }
    int    ValueLen( void ) { return valueLen; }
    int    IsNull( void ) { return isNull; }

    void    SetName( const char * const _name, int &count );


  private:

    char      *name;
    int        datatype;
    int        format;
    int        maxlength;
    int        scale;
    int        precision;
    int        status;

    int        valueLen;
    char      *value;
    int        isNull;

    int        count;
    int        userType;
};

#ifdef _pd123_
#define Status _pd123_ 
#undef _pd123_
#endif

#endif /* _DBHOSTVARIABLE_H_ */	



