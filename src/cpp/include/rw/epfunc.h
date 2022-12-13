#ifndef __RWEPFUNC_H__
#define __RWEPFUNC_H__

/***************************************************************************
 *
 * epfunc.h - work-around classes for problems with some STL functors 
 *   
 * $Id: epfunc.h 18623 2000-03-03 08:28:13Z sih $
 *
 * Copyright (c) 1989-1999 Rogue Wave Software, Inc.  All Rights Reserved.
 *
 * This computer software is owned by Rogue Wave Software, Inc. and is
 * protected by U.S. copyright laws and other laws and by international
 * treaties.  This computer software is furnished by Rogue Wave Software,
 * Inc. pursuant to a written license agreement and may be used, copied,
 * transmitted, and stored only in accordance with the terms of such
 * license and with the inclusion of the above copyright notice.  This
 * computer software or any other copies thereof may not be provided or
 * otherwise made available to any other person.
 *
 * U.S. Government Restricted Rights.  This computer software is provided
 * with Restricted Rights.  Use, duplication, or disclosure by the
 * Government is subject to restrictions as set forth in subparagraph (c)
 * (1) (ii) of The Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013 or subparagraphs (c) (1) and (2) of the
 * Commercial Computer Software – Restricted Rights at 48 CFR 52.227-19,
 * as applicable.  Manufacturer is Rogue Wave Software, Inc., 5500
 * Flatiron Parkway, Boulder, Colorado 80301 USA.
 *
 **************************************************************************/

#include "rw/compiler.h"
#include "rw/defs.h"

#ifndef RW_HEADER_WITH_EXTENSION
#  include <functional>
#else
#  include <functional.hpp>
#endif  /* RW_HEADER_WITH_EXTENSION */

template <class Arg1, class Arg2, class Result>
class x_pointer_to_binary_function : public RW_SL_STD(binary_function)<Arg1, Arg2, Result>
{ 
protected:
    Result (*ptr)(Arg1, Arg2);
public:
    x_pointer_to_binary_function(Result (*x)(Arg1, Arg2)) : ptr(x) {}
    Result operator()(const Arg1 x, const Arg2 y) const {
        return ptr(x, y); 
    }
};


template <class Operation> 
class x_binder2nd : public RW_SL_STD(unary_function)<typename Operation::first_argument_type,
                                          typename Operation::result_type> {
protected:
    Operation op;
    typename Operation::second_argument_type value;
public:
    x_binder2nd(const Operation& x, const typename Operation::second_argument_type y) 
        : op(x), value(y) {}
    
#ifdef RW_NO_CONST_PARAM_OVERLOAD
    typename Operation::result_type operator()(argument_type x) const
#else
    typename Operation::result_type operator()(const typename
                 RW_SL_STD(unary_function)<typename Operation::first_argument_type,
                 typename Operation::result_type>::argument_type x) const
#endif
                 {return op(x, value);}
};

template <class Operation, class T>
x_binder2nd<Operation> x_bind2nd(const Operation& op, const T& x) {
    return x_binder2nd<Operation>(op, Operation::second_argument_type(x));
}

#endif /* __RWEPFUNC_H__ */
