
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Returns true of the object is of the specified type.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_IS_OF_TYPE_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_IS_OF_TYPE_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Group.h"


namespace AFW {
namespace Conditions {


template < class T > class IsOfType : public Group
{
public:

  // Typedefs.
  typedef Group BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( IsOfType );

  // Type-id definition.
  USUL_IMPLEMENT_INLINE_TYPE_ID ( IsOfType );

  // Constructor
  IsOfType()
  {
  }

  // Returns true of the object is of the specified type.
  virtual bool evaluate ( AFW::Core::Object *object = 0x0 )
  {
    return ( 0x0 != dynamic_cast < T * > ( object ) );
  }

protected:

  // Use reference counting.
  virtual ~IsOfType()
  {
  }

private:

  // No copying.
  IsOfType ( const IsOfType & );
  IsOfType &operator = ( const IsOfType & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_IS_OF_TYPE_CONDITION_CLASS_H_
