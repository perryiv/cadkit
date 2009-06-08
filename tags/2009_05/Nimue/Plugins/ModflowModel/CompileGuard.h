
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This header file should not be included in client code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_COMPILE_GUARD_H_
#define _MODFLOW_MODEL_COMPILE_GUARD_H_

#ifndef _COMPILING_MODFLOW_MODEL
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#endif // _MODFLOW_MODEL_COMPILE_GUARD_H_
