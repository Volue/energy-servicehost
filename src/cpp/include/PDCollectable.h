/*
$Header$
Description:
    This class is the baseclass for collectable objects.
    It is an extension to RWCollectable and supports
    isAKindOf(CLSID) function needed for typesafe downcasting.
    Macros are provided in order to allow for use of the
    C++ dynamic_cast operator.
    Thus compiling with CPPRTI enabled should result in using
    the C++ language definitions

How to use:
    Ref. RW users guide for RWCollectable
    AND this:
    Example 1: "Simple inheritance directly from RWCollectable"
    #include <PDCollectable.h>
    class YourClass: public PDCollectable 'before: RWCollectable'  {
        public:
        //before:RWDECLARE_COLLECTABLE(YourClass)
        PDDECLARE_COLLECTABLE(YourClass,PDCollectable)
    };
    Example 2: "Y is a simple inheritance from class X, that is a RWCollectable"
    a) Apply step as described in example 1 to class X. i.e.:
        class X: public PDCollectable {
          public:
          PDDECLARE_COLLECTABLE(X,PDCollectable)
        };
    b) Modify class Y like this:
        class Y: public X {
            public:
            //before: RWDECLARE_COLLECTABLE(Y)
            PDDECLARE_COLLECTABLE(Y,X)
        }
    Example 3: "Z is multiple inheritance from X and Y that both are PDCollectable"
    a) Assume that X and Y are PDCollectable as described above.
    b) 
        class Z: public X, public Y {
            public:
            PDDECLARE_COLLECTABLE2(Z,X,Y)
        };
    
Copyright (C) Powel Data AS
All rights reserved.
*/

#ifndef _PDCollectable_h_
#define _PDCollectable_h_

#include <rw/collect.h>

//---- PDCollectable -----------------------------------------------------------
#ifndef __CPPRTTI
class PDCollectable : public RWCollectable {
  public:
    virtual RWBoolean isAKindOf(RWClassID x) { return x == RWCollectable::classIsA(); }

};
#define PDDEFINE_ISAKINDOF(thisClass,baseClass) virtual RWBoolean isAKindOf(RWClassID id) {\
    if(id == myAtom) return TRUE;\
    else return baseClass::isAKindOf(id);\
}

#define PDDEFINE_ISAKINDOF2(thisClass,baseClass1,baseClass2) virtual RWBoolean isAKindOf(RWClassID id) {\
    if(id == myAtom) return TRUE;\
    else if( baseClass1::isAKindOf(id)) return TRUE;\
    else return  baseClass2::isAKindOf(id);\
}

#define PDDECLARE_COLLECTABLE(thisClass,baseCls) RWDECLARE_COLLECTABLE(thisClass) PDDEFINE_ISAKINDOF(thisClass,baseCls)
#define pd_dynamic_cast_macro(cls,expr) ((expr)?((expr)->isAKindOf(cls::classIsA()) ?(cls*)(expr):(cls*)0):(cls*)0)
#ifndef __alpha
// this one does not have side-effects
// but for alpha/unix, we dont have code that
// gives possible side-effects.
template <class cls> 
    cls * pd_dynamic_cast_impl(PDCollectable *x) {
	if(x) {
	    if(x->isAKindOf(cls::classIsA()) )
		return (cls*) x;
	}
	return (cls*)0;
    }
 
#define pd_dynamic_cast(cls,expr) pd_dynamic_cast_impl<cls>(expr)
#else
#define pd_dynamic_cast(cls,expr) pd_dynamic_cast_macro(cls,expr)
#endif
#else
typedef RWCollectable PDCollectable
#define PDDECLARE_COLLECTABLE(thisCls,baseCls) RWDECLARE_COLLECTABLE(thisCls)
#define pd_dynamic_cast(cls,expr) dynamic_cast<cls*>(expr)
#define pd_dynamic_cast_macro(cls,expr) dynamic_cast<cls*>(expr)
#endif
#endif
