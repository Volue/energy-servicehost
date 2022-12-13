//-*-Mode: C++;-*-
#ifndef _CBBase_h_
#define _CBBase_h_

#if __GNUG__ >= 2
#  pragma interface
#endif


//---- CBBase -----------------------------------------------------------
template <class CBargtype>
class CBBase
{ // used by the CBModel derived classes
public:
    CBBase() {};
    virtual ~CBBase() {};
    virtual void operator()(CBargtype) = 0;
};

template <class CBtarget, class CBargtype>
class CBX:public CBBase<CBargtype> 
{ // implementation (by target class)
    CBtarget *_object;
    void (CBtarget::*_method)(CBargtype);
public:
    CBX() : CBBase<CBargtype>() 
    {
        _object = nullptr;
        _method = nullptr;
    }
    CBX(CBtarget * object, void(CBtarget:: * method)(CBargtype)) : CBBase<CBargtype>() 
    {
        _object = object;
        _method = method;
    }
    virtual void operator() (CBargtype arg) 
    {
        if (_object != nullptr && _method != nullptr)
            (_object->*_method)(arg);
    }
};

// CBExObject class is added to overcome problems with excessive memory consumption 
// by TsRBModel::_funcImplementationMap function map,
// which uses CBX and TsRFuncImpl objects:
template <class CBtarget, class CBargtype>
class CBExObject // :public CBBase<CBargtype> 
{
    void (CBtarget::* _method)(CBargtype);
public:
    // CBExObject():CBBase<CBargtype>() { _method = nullptr; }
    CBExObject() { _method = nullptr; }
    CBExObject(void(CBtarget::*method)(CBargtype)) { _method = method; }
    // virtual void operator() (CBargtype) { throw exception("CBExObject::operator() (CBargtype) cannot be used. Use operator() (CBtarget & object, CBargtype args) instead"); };
    virtual void operator() (CBtarget & object, CBargtype args)
    {
        if (_method != nullptr)
            (object.*_method)(args);
    }
};

template <class CBargtype1, class CBargtype2>
class CBBase2 
{
public:
    CBBase2() {};
    virtual ~CBBase2() {};
    virtual void operator()(CBargtype1 arg1, CBargtype2 arg2) = 0;
};

template <class CBtarget, class CBargtype1, class CBargtype2>
class CBX2: public CBBase2<CBargtype1, CBargtype2>
{
    CBtarget * _object;
    void (CBtarget::*_method)(CBargtype1, CBargtype2);
public:
    CBX2(): CBBase2<CBargtype1, CBargtype2>()
    { 
        _object = nullptr; 
        _method = nullptr; 
    }
    CBX2(CBtarget * object, void(CBtarget:: * method)(CBargtype1, CBargtype2)): CBBase2<CBargtype1, CBargtype2>()
    {
        _object = object;
        _method = method;
    }
    virtual void operator()(CBargtype1 arg1, CBargtype2 arg2) 
    {
        if (_object != nullptr && _method != nullptr)
            (_object->*_method)(arg1, arg2);
    }
};
#endif
