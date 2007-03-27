
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Condition that is always true.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_ALWAYS_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_ALWAYS_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Condition.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT Always : public Condition
{
public:

  // Typedefs.
  typedef Condition BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Always );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Always );

  // Constructor
  Always();

  // The condition is always met.
  virtual bool                    evaluate ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~Always();

private:

  // No copying.
  Always ( const Always & );
  Always &operator = ( const Always & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_ALWAYS_CONDITION_CLASS_H_
