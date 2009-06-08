
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Condition that is never true.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_NEVER_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_NEVER_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Condition.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT Never : public Condition
{
public:

  // Typedefs.
  typedef Condition BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Never );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Never );

  // Constructor
  Never();

  // The condition is never met.
  virtual bool                    evaluate ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~Never();

private:

  // No copying.
  Never ( const Never & );
  Never &operator = ( const Never & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_NEVER_CONDITION_CLASS_H_
