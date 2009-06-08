
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Macros used in object classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_OBJECTS_MACROS_H_
#define _OSG_TOOLS_OBJECTS_MACROS_H_

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Base/Typed.h"
#include "Usul/Preprocess/UniqueName.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Scope/Caller.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Put in class definition.
//
///////////////////////////////////////////////////////////////////////////////

#define OSG_TOOLS_OBJECTS_DECLARE_CLASS(class_name)\
  USUL_DECLARE_TYPE_ID ( class_name );\
  USUL_DECLARE_REF_POINTERS ( class_name )


///////////////////////////////////////////////////////////////////////////////
//
//  Put in class's cpp file.
//
///////////////////////////////////////////////////////////////////////////////

#define OSG_TOOLS_OBJECTS_IMPLEMENT_CLASS(class_name)\
  USUL_IMPLEMENT_TYPE_ID ( class_name )


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience macros used to always call the function when the scope ends.
//
///////////////////////////////////////////////////////////////////////////////

#define OSG_TOOLS_SCOPE_END_CALL_1(function_name,the_argument)\
  Usul::Scope::Caller::RefPtr USUL_UNIQUE_NAME ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 \
    ( the_argument, function_name ) ) )

#define OSG_TOOLS_SCOPE_END_CALL_2(function_name,first_argument,second_argument)\
  Usul::Scope::Caller::RefPtr USUL_UNIQUE_NAME ( Usul::Scope::makeCaller ( Usul::Adaptors::bind2 \
    ( first_argument, second_argument, function_name ) ) )


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience macro used to always set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

#define OSG_TOOLS_SCOPE_END_SET_DIRTY_STATE(dirty_flag_name,dirty_state)\
  OSG_TOOLS_SCOPE_END_CALL_2 ( Usul::Adaptors::memberFunction ( this, &Object::setDirty ), Dirty::VERTICES, false )

#endif // _OSG_TOOLS_OBJECTS_MACROS_H_
