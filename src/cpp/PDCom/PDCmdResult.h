/*
$Header$
Description:
 Echo Request message
 
Copyright (C) Powel Data AS
All rights reserved.
*/
#ifndef _PDCmdResult_h_
#define _PDCmdResult_h_

#include <PDCollectable.h>
#include <rw/cstring.h>
//---- PDCmdResult -----------------------------------------------------------

class PDCmdResult : public PDCollectable {
  public:
    PDCmdResult();
    PDCmdResult(RWCString x, RWBoolean y);
    ~PDCmdResult();
//    RWCString getMsg() { return ErrMsg;}
//    void setMsg(RWCString x) { ErrMsg=x;}
//    void setMsg(const char *x) { ErrMsg=x;}
    PDDECLARE_COLLECTABLE(PDCmdResult,PDCollectable)
    virtual RWspace binaryStoreSize() const;
    virtual void restoreGuts(RWvistream&);
    virtual void restoreGuts(RWFile&);
    virtual void saveGuts(RWvostream&) const;
    virtual void saveGuts(RWFile&) const;
    RWCString ErrMsg;
    RWBoolean Success;

  protected:
};

#endif
