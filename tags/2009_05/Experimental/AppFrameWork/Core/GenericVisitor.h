
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Generic visitor class and helper function.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_GENERIC_VISITOR_CLASS_H_
#define _APP_FRAME_WORK_GENERIC_VISITOR_CLASS_H_

#include "AppFrameWork/Core/BaseVisitor.h"


namespace AFW {
namespace Core {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic visitor class.
//
///////////////////////////////////////////////////////////////////////////////

template 
<
  class ObjectType_, 
  class FunctionType_
>
struct GenericVisitor : public AFW::Core::BaseVisitor
{
  // Typedefs.
  typedef AFW::Core::BaseVisitor BaseClass;
  typedef ObjectType_ ObjectType;
  typedef FunctionType_ FunctionType;
  typedef GenericVisitor < ObjectType, FunctionType > ThisType;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ThisType );

  GenericVisitor ( const FunctionType &function ) : BaseClass(),
    _function ( function )
  {
  }

  virtual void visit ( ObjectType *object )
  {
    _function ( object );
    BaseClass::visit ( object );
  }

  const FunctionType &  function() const { return _function; }
  FunctionType &        function()       { return _function; }

protected:

  // Use reference counting.
  virtual ~GenericVisitor()
  {
  }

private:

  // No copying.
  GenericVisitor ( const GenericVisitor & );
  GenericVisitor &operator = ( const GenericVisitor & );

  FunctionType _function;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to make the visitor.
//
///////////////////////////////////////////////////////////////////////////////

template < class ObjectType_ > struct MakeVisitor
{
  template < class FunctionType_ >
  static GenericVisitor<ObjectType_,FunctionType_> *make ( const FunctionType_ &function )
  {
    return new GenericVisitor<ObjectType_,FunctionType_> ( function );
  }
};


} // namespace Core
} // namespace AFW


#endif // _APP_FRAME_WORK_GENERIC_VISITOR_CLASS_H_
