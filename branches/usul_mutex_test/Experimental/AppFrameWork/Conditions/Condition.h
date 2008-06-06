
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all conditions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_BASE_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_BASE_CONDITION_CLASS_H_

#include "AppFrameWork/Core/Object.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT Condition : public AFW::Core::Object
{
public:

  // Typedefs.
  typedef AFW::Core::Object BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Condition );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Condition );

  // See if the condition is met.
  virtual bool                    evaluate ( AFW::Core::Object *object = 0x0 ) = 0;

protected:

  // Constructor
  Condition ( bool want = true );

  // Use reference counting.
  virtual ~Condition();

  bool _want;

private:

  // No copying.
  Condition ( const Condition & );
  Condition &operator = ( const Condition & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_BASE_CONDITION_CLASS_H_
