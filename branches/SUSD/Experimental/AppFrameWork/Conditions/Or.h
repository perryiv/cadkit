
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  One of the contained conditions has to be true.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_OR_CONDITION_CLASS_H_
#define _APP_FRAME_WORK_OR_CONDITION_CLASS_H_

#include "AppFrameWork/Conditions/Group.h"


namespace AFW {
namespace Conditions {


class APP_FRAME_WORK_EXPORT Or : public Group
{
public:

  // Typedefs.
  typedef Group BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Or );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Or );

  // Constructor
  Or();

  // One of the contained conditions has to be true.
  virtual bool                    evaluate ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~Or();

private:

  // No copying.
  Or ( const Or & );
  Or &operator = ( const Or & );
};


} // namespace Conditions
} // namespace AFW


#endif //_APP_FRAME_WORK_OR_CONDITION_CLASS_H_
