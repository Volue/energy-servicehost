/*
$Header$
Description:
 Echo Request message
 
Copyright (C) Powel Data AS
All rights reserved.
*/
#ifndef _PDEchoReply_h_
#define _PDEchoReply_h_

#include <PDCollectable.h>
#include <rw/cstring.h>
//---- PDEchoReply -----------------------------------------------------------

class PDEchoReply : public PDCollectable {
  public:
    PDEchoReply();
    PDEchoReply(RWCString x);
    ~PDEchoReply();
    RWCString getMsg() { return echoMsg;}
    void setMsg(RWCString x) { echoMsg=x;}
    void setMsg(const char *x) { echoMsg=x;}
    PDDECLARE_COLLECTABLE(PDEchoReply,PDCollectable)
    virtual RWspace binaryStoreSize() const;
    virtual void restoreGuts(RWvistream&);
    virtual void restoreGuts(RWFile&);
    virtual void saveGuts(RWvostream&) const;
    virtual void saveGuts(RWFile&) const;
    
  protected:
    RWCString echoMsg;
};

#endif
